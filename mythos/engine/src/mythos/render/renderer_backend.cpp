#include <mythos/log.hpp>
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>

namespace myth {
    MYL_NO_DISCARD auto renderer_backend::create(render_api api, const renderer_configuration& config) -> std::unique_ptr<renderer_backend> {
        switch (api) {
            using enum render_api;
            case vulkan:
                return std::make_unique<myth::vulkan::backend>(config);
            case none:
                MYTHOS_FATAL("No render API selected");
                return nullptr;
            default:
                MYTHOS_FATAL("Unknown render API");
                return nullptr;
        }
    }
}