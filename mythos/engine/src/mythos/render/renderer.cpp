#include <mythos/assert.hpp>
#include <mythos/render/renderer.hpp>
#include <mythos/render/shader.hpp>

#include <myl/platform.hpp>
#include <myl/math/vec4.hpp>
#include <myl/memory.hpp>

namespace myth {
    struct triangle_vertex {
        myl::f32vec2 position;
        myl::f32vec4 color;
    };

    struct renderer_data {
        std::unique_ptr<shader>              triangle_shader = nullptr; /// MYTODO: ALL SHADERS SHOULD BE SHARED PTR INCASE OF DOUBLE LOAD
        std::unique_ptr<render_buffer>       triangle_vbo = nullptr;
        myl::buffer<std::allocator<myl::u8>> triangle_vertex_data;
        myl::u32                             triangle_vertex_count = 0;
        const myl::u32                       triangle_max_vertex_count = 3 * 1024;
    };

    struct internal_shader {
        const char* faux_path;
        const char* data;
    };

    constexpr internal_shader triangle_shader_info{
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

        g_rd.triangle_shader = shader::create(triangle_shader_info.faux_path, triangle_shader_info.data, shader_primitive::triangle);
        const myl::usize triangle_vertices_byte_count = g_rd.triangle_max_vertex_count * sizeof(triangle_vertex);
        g_rd.triangle_vbo = s_backend->create_render_buffer(render_buffer_usage::vertex, triangle_vertices_byte_count);
        g_rd.triangle_vertex_data.allocate(triangle_vertices_byte_count);
        
        triangle_vertex vertices[]{ /// MYTemp: To be removed upon addition of index rendering
            {{ 0.f, -.5f }, { 1.f, 1.f, 1.f, 1.f }},
            {{ .5f, .5f },  { 0.f, 1.f, 0.f, 1.f }},
            {{ -.5f, .5f }, { 0.f, 0.f, 1.f, 1.f }}
        };
        
        g_rd.triangle_vertex_data.set(vertices, 3 * sizeof(triangle_vertex));
        g_rd.triangle_vertex_count = 3;

        g_rd.triangle_vbo->upload(g_rd.triangle_vertex_data.data(), g_rd.triangle_vertex_count * sizeof(triangle_vertex));
    }

    auto renderer::shutdown() -> void {
        s_backend->prepare_shutdown();

        s_backend->destroy_render_buffer(g_rd.triangle_vbo.get()); /// This may need to be called between swapchain destroy and device destroy
        // g_rd.triangle_vertex_data will automatically delete data
        shader::destroy(g_rd.triangle_shader);

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
        draw_data draw_data{
            .shader        = *g_rd.triangle_shader.get(),
            .vertex_buffer = *g_rd.triangle_vbo.get(),
            .vertex_count  = g_rd.triangle_vertex_count
        };

        renderer::backend()->draw(draw_data);
    }
}
