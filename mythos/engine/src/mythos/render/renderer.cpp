#include <mythos/assert.hpp>
#include <mythos/render/renderer.hpp>

#include <myl/platform.hpp>

#include <mythos/render/shader.hpp>

namespace myth {
    struct internal_shader {
        const char* faux_path;
        const char* data;
    };

    constexpr internal_shader triangle_shader{
        .faux_path = "resources/shaders/mythos_internal_triangle.glsl",
        .data =
            "#type vertex\r\n"
            "#version 460 core\r\n"
            
            "struct vertex_output {\r\n"
            "    vec4 color;\r\n"
            "};\r\n"
            
            "layout(location = 0) out vertex_output o_output;\r\n"
            
            "vec2 positions[3] = vec2[](\r\n"
            "   vec2(0.0, -0.5),\r\n"
            "   vec2(0.5, 0.5),\r\n"
            "   vec2(-0.5, 0.5)\r\n"
            ");\r\n"
            
            "vec4 colors[3] = vec4[](\r\n"
            "   vec4(1.0, 0.0, 0.0, 1.0),\r\n"
            "   vec4(0.0, 1.0, 0.0, 1.0),\r\n"
            "   vec4(0.0, 0.0, 1.0, 1.0)\r\n"
            ");\r\n"
            
            "void main() {\r\n"
            "    o_output.color = colors[gl_VertexIndex];\r\n"
            "    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);\r\n"
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

    MYL_NO_DISCARD static constexpr auto select_render_api() noexcept -> render_api {
        using enum render_api;
#ifdef MYL_OS_WINDOWS
        return vulkan;
#else
#   warning "No default render api set for this platform"
        return none;
#endif
    }

    /// MYTEMP:
    ///std::unique_ptr<shader> s_triangle_shader = nullptr;
    /// MYTEMP:

    std::unique_ptr<renderer_backend> renderer::s_backend = nullptr;
    render_api renderer::s_api = render_api::none;

    auto renderer::init(const renderer_configuration& config) -> void {
        MYTHOS_TRACE("Initializing renderer...");
        s_api = select_render_api();
        MYTHOS_VERIFY(s_api != render_api::none);
        MYTHOS_INFO("Render API '{}' selected", render_api_to_string(s_api));

        s_backend = renderer_backend::create(s_api, config);
        MYTHOS_TRACE("Renderer initialized");

        ///s_triangle_shader = shader::create(triangle_shader.faux_path, triangle_shader.data, shader_primitive::triangle);
    }

    auto renderer::shutdown() -> void {
        s_backend->prepare_shutdown();

        ///s_triangle_shader.reset();

        MYTHOS_TRACE("Terminating renderer...");
        s_backend.reset();
        MYTHOS_TRACE("Renderer terminated");
    }

    auto renderer::begin_frame() -> bool {
        return s_backend->begin_frame();
    }

    auto renderer::end_frame() -> void {
        s_backend->end_frame();
    }

    auto draw() -> void {
        ///s_triangle_shader->bind();

        ///renderer::backend()->draw();
    }
}
