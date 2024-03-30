#pragma once
#include <myl/math/vec3.hpp>

#include <memory>

namespace myth {
    enum class render_api {
        vulkan,
        none
    };

    struct renderer_configuration {
        myl::f32vec3 clear_color = myl::f32vec3(0.f);
        /// bool vsync = true
    };

    class renderer_backend {
    public:
        MYL_NO_DISCARD static auto create(render_api api, const renderer_configuration& config) -> std::unique_ptr<renderer_backend>;

        virtual ~renderer_backend() = default;
    };
}
