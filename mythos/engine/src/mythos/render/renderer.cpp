#include <mythos/assert.hpp>
#include <mythos/render/renderer.hpp>
#include <mythos/render/shader.hpp>

#include <myl/platform.hpp>
#include <myl/math/vec4.hpp>
#include <myl/memory.hpp>

namespace myth {
    struct vertex {
        myl::f32vec2 position;
        myl::f32vec4 color;
    };

    struct renderer_data {
        const myl::u32 max_quads = 1024;
        const myl::u32 max_quad_vertices = max_quads * 4;
        const myl::u32 max_quad_indices = max_quads * 6;

        std::unique_ptr<shader>              quad_shader = nullptr; /// MYTODO: ALL SHADERS SHOULD BE SHARED PTR INCASE OF DOUBLE LOAD
        myl::buffer<std::allocator<myl::u8>> quad_vertex_data;
        std::unique_ptr<render_buffer>       quad_vbo = nullptr;
        myl::u32                             quad_vertex_count = 0; /// MYTEMP: Until batching is introduced
        std::unique_ptr<render_buffer>       quad_ibo = nullptr;
        myl::u32                             quad_index_count = 0;
    };

    struct internal_shader {
        const char* faux_path;
        const char* data;
    };

    constexpr internal_shader quad_shader_info{
        .faux_path = "resources/shaders/mythos_internal_triangle.glsl",
        .data =
            "#type vertex\r\n"
            "#version 460 core\r\n"
            
            "struct vertex_output {\r\n"
            "    vec4 color;\r\n"
            "};\r\n"
                        
            "layout(location = 0) in vec2 i_position;\r\n"
            "layout(location = 1) in vec4 i_color;\r\n"

            "layout(location = 0) out vertex_output o_output;\r\n"
            
            "void main() {\r\n"
            "    o_output.color = i_color;\r\n"
            "    gl_Position = vec4(i_position, 0.0, 1.0);\r\n"
            "}\r\n"
            
            "#type fragment\r\n"
            "#version 460 core\r\n"
            
            "struct vertex_input {\r\n"
            "    vec4 color;\r\n"
            "};\r\n"
            
            "layout(location = 0) in vertex_input i_input;\r\n"
            
            "layout(location = 0) out vec4 o_color;\r\n"
            
            "void main() {\r\n"
            "    o_color = i_input.color;\r\n"
            "}"
    };

    MYL_NO_DISCARD consteval auto select_render_api() noexcept -> render_api {
        using enum render_api;
#ifdef MYL_OS_WINDOWS
        return vulkan;
#else
#   warning "No default render api set for this platform"
        return none;
#endif
    }

    std::unique_ptr<renderer_backend> renderer::s_backend = nullptr;
    render_api renderer::s_api = render_api::none;

    renderer_data g_rd{};

    auto renderer::init(const renderer_configuration& config) -> void {
        MYTHOS_TRACE("Initializing renderer...");
        s_api = select_render_api();
        MYTHOS_VERIFY(s_api != render_api::none);
        MYTHOS_INFO("Render API '{}' selected", render_api_to_string(s_api));

        s_backend = renderer_backend::create(s_api, config);
        MYTHOS_TRACE("Renderer initialized");        

        // Quad

        g_rd.quad_shader = shader::create(quad_shader_info.faux_path, quad_shader_info.data, shader_primitive::triangle);

        const myl::usize quad_vertices_byte_size = g_rd.max_quad_vertices * sizeof(vertex);
        g_rd.quad_vbo = s_backend->create_render_buffer(render_buffer_usage::vertex, quad_vertices_byte_size);

        vertex quad_vertices[]{ /// MYTemp: To be removed upon addition of index rendering
            {{ -.5f, -.5f }, { 1.f, 0.f, 0.f, 1.f }},
            {{  .5f, -.5f }, { 0.f, 1.f, 0.f, 1.f }},
            {{  .5f,  .5f }, { 0.f, 0.f, 1.f, 1.f }},
            {{ -.5f,  .5f }, { 1.f, 1.f, 1.f, 1.f }}
        };
        
        g_rd.quad_vertex_data.allocate(quad_vertices_byte_size);
        g_rd.quad_vertex_data.set(quad_vertices, 4 * sizeof(vertex));
        g_rd.quad_vertex_count = 4;

        g_rd.quad_vbo->upload(g_rd.quad_vertex_data.data(), g_rd.quad_vertex_count * sizeof(vertex));

        std::vector<uint16_t> quad_indices(g_rd.max_quad_indices); /// MYTODO: increase vertices limit, this will max at 65535
        for (myl::u32 i = 0, offset = 0; i != g_rd.max_quad_indices; i += 6, offset += 4) {
            quad_indices[i + 0] = offset + 0;
            quad_indices[i + 1] = offset + 1;
            quad_indices[i + 2] = offset + 2;
            quad_indices[i + 3] = offset + 2;
            quad_indices[i + 4] = offset + 3;
            quad_indices[i + 5] = offset + 0;
        }

        const myl::usize quad_indices_byte_size = g_rd.max_quad_indices * sizeof(uint16_t);
        g_rd.quad_ibo = s_backend->create_render_buffer(render_buffer_usage::index, quad_indices_byte_size);
        g_rd.quad_ibo->upload(quad_indices.data(), quad_indices_byte_size);
        g_rd.quad_index_count = 6; /// Temp: should not be done here
    }

    auto renderer::shutdown() -> void {
        s_backend->prepare_shutdown();

        s_backend->destroy_render_buffer(g_rd.quad_ibo.get());
        s_backend->destroy_render_buffer(g_rd.quad_vbo.get());

        g_rd.quad_vertex_data.deallocate();

        shader::destroy(g_rd.quad_shader);

        MYTHOS_TRACE("Terminating renderer...");
        s_backend.reset();
        MYTHOS_TRACE("Renderer terminated");
    }

    auto renderer::set_clear_color(const myl::f32vec3& color) -> void {
        s_backend->set_clear_color(color);
    }

    auto renderer::set_vsync(bool value) -> void {
        s_backend->set_vsync(value);
    }

    auto renderer::begin_frame() -> bool {
        return s_backend->begin_frame();
    }

    auto renderer::end_frame() -> void {
        s_backend->end_frame();
    }

    auto renderer::draw() -> void {     
        indexed_draw_data indexed_draw_data{
            .shader        = *g_rd.quad_shader.get(),
            .vertex_buffer = *g_rd.quad_vbo.get(),
            .vertex_count  = g_rd.quad_vertex_count,
            .index_buffer  = *g_rd.quad_ibo.get(),
            .index_count   = g_rd.quad_index_count
        };

        renderer::backend()->draw(indexed_draw_data);
    }
}
