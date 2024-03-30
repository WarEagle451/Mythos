#include <mythos/assert.hpp>
#include <mythos/render/renderer.hpp>

#include <myl/platform.hpp>

namespace myth {
    MYL_NO_DISCARD constexpr auto render_api_name(render_api api) -> const char* {
        switch (api) {
            using enum render_api;
            case vulkan: return "Vulkan";
            case none:   return "None";
            default:     return "Unknown";
        }
    }

    MYL_NO_DISCARD constexpr auto select_render_api() noexcept -> render_api {
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

    auto renderer::init(const renderer_configuration& config) -> void {
        MYTHOS_TRACE("Initializing renderer...");
        s_api = select_render_api();
        MYTHOS_VERIFY(s_api != render_api::none);
        MYTHOS_INFO("Render API '{}' selected", render_api_name(s_api));

        s_backend = renderer_backend::create(s_api, config);
        MYTHOS_TRACE("Renderer initialized");
    }

    auto renderer::shutdown() -> void {
        MYTHOS_TRACE("Terminating renderer...");
        s_backend.reset();
        MYTHOS_TRACE("Renderer terminated");
    }
}
