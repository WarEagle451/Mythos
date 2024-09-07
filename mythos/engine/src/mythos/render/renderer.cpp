#include <mythos/assert.hpp>
#include <mythos/render/renderer.hpp>
#include <mythos/render/shader.hpp>

#include <myl/platform.hpp>
#include <myl/memory.hpp>

/// MYBUG: Successive draw calls within the same frame will overwrite undrawn data if it occupies the same memory.
/// Eg. draw call 1: AAooo | draw call 2: Booo -> drawn to screen as BAoo
/// Best guess is that the previous draw call is not drawn immediately when told to or is still in the process of drawing.
/// Possibly solved by adding a fence to the renderers draw command per shader type
/// Could also be an issued if I do not understand drawing to work like I think I do

namespace myth {
    /// MYTodo: Remove vertex structs at some point, should be automatically generated from shader code

    struct vertex {
        myl::f32vec2 position;
        myl::f32vec4 color;
    };

    struct renderer_data {
        const myl::u32 max_quads = 1024;
        const myl::u32 max_quad_vertices = max_quads * 4;
        const myl::u32 max_quad_indices = max_quads * 6;
        const myl::f32vec2 local_quad_vertex_positions[4] = { /// MYTODO: Will need to be changed to f32vec3 if 3d
            { -.5f, -.5f },
            {  .5f, -.5f },
            {  .5f,  .5f },
            { -.5f,  .5f }
        };

        std::unique_ptr<shader>              quad_shader = nullptr; /// MYTODO: ALL SHADERS SHOULD BE SHARED PTR INCASE OF DOUBLE LOAD
        myl::buffer<std::allocator<myl::u8>> quad_vertex_data;
        vertex*                              quad_vertex_data_ptr;
        std::unique_ptr<render_buffer>       quad_vbo = nullptr;
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

        // Quad

        g_rd.quad_shader = shader::create(quad_shader_info.faux_path, quad_shader_info.data, shader_primitive::triangle);

        const myl::usize quad_vertices_buffer_size = g_rd.max_quad_vertices * sizeof(vertex);
        g_rd.quad_vertex_data.allocate(quad_vertices_buffer_size);
        g_rd.quad_vertex_data_ptr = g_rd.quad_vertex_data.as<vertex>();

        g_rd.quad_vbo = s_backend->create_render_buffer(render_buffer_usage::vertex, quad_vertices_buffer_size);

        std::vector<uint32_t> quad_indices(g_rd.max_quad_indices);
        const myl::usize quad_indices_buffer_size = g_rd.max_quad_indices * sizeof(decltype(quad_indices)::value_type);
        g_rd.quad_ibo = s_backend->create_render_buffer(render_buffer_usage::index, quad_indices_buffer_size);
        for (myl::u32 i = 0, offset = 0; i != g_rd.max_quad_indices; i += 6, offset += 4) {
            quad_indices[i + 0] = offset + 0;
            quad_indices[i + 1] = offset + 1;
            quad_indices[i + 2] = offset + 2;
            quad_indices[i + 3] = offset + 2;
            quad_indices[i + 4] = offset + 3;
            quad_indices[i + 5] = offset + 0;
        }
        g_rd.quad_ibo->upload(quad_indices.data(), quad_indices_buffer_size);

        g_rd.quad_index_count = 0;

        MYTHOS_TRACE("Renderer initialized");
    }

    auto renderer::shutdown() -> void {
        s_backend->prepare_shutdown();

        s_backend->destroy_render_buffer(g_rd.quad_ibo.get());
        s_backend->destroy_render_buffer(g_rd.quad_vbo.get());

        g_rd.quad_vertex_data.deallocate();
        g_rd.quad_vertex_data_ptr = nullptr;

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

    auto renderer::begin_quad_batch() -> void {
        g_rd.quad_index_count = 0;
        g_rd.quad_vertex_data_ptr = g_rd.quad_vertex_data.as<vertex>();
    }

    auto renderer::begin_batch() -> void {
        begin_quad_batch();
    }

    auto renderer::flush_quad_batch() -> void {
        if (g_rd.quad_index_count != 0) {
            const myl::usize size = static_cast<myl::usize>(reinterpret_cast<uint8_t*>(g_rd.quad_vertex_data_ptr) - reinterpret_cast<uint8_t*>(g_rd.quad_vertex_data.data()));
            g_rd.quad_vbo->upload(g_rd.quad_vertex_data.data(), size);          

            indexed_draw_data indexed_draw_data{
                .shader        = *g_rd.quad_shader.get(),
                .vertex_buffer = *g_rd.quad_vbo.get(),
                .index_buffer  = *g_rd.quad_ibo.get(),
                .index_count   = g_rd.quad_index_count
            };

            renderer::backend()->draw(indexed_draw_data);
        }
    }

    auto renderer::flush_batch() -> void {
        flush_quad_batch();
    }

    auto renderer::next_quad_batch() -> void {
        flush_quad_batch();
        begin_quad_batch();
    }

    auto renderer::next_batch() -> void {
        flush_batch();
        begin_batch();
    }

    auto renderer::begin() -> void {
        begin_batch();
    }

    auto renderer::end() -> void {
        flush_batch();
    }

    auto renderer::draw_quad(const myl::f32vec2& pos, const myl::f32vec4& color) -> void {
        if (g_rd.max_quad_indices < g_rd.quad_index_count)
            next_quad_batch();

        for (myl::usize i = 0; i != 4; ++i) {
            /// MYTODO: Below will have to be changed if transforms are to be used
            g_rd.quad_vertex_data_ptr->position = pos + g_rd.local_quad_vertex_positions[i];
            g_rd.quad_vertex_data_ptr->color = color;

            ++g_rd.quad_vertex_data_ptr;
        }

        g_rd.quad_index_count += 6;
    }
}
