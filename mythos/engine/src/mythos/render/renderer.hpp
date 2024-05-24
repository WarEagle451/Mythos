#pragma once
#include <mythos/render/renderer_backend.hpp>

namespace myth {
    class renderer {
        static render_api s_api;
        static std::unique_ptr<renderer_backend> s_backend;
    public:
        static auto init(const renderer_configuration& config) -> void;
        static auto shutdown() -> void;

        static auto backend() noexcept -> renderer_backend* { return s_backend.get(); }

        MYL_NO_DISCARD static MYL_API auto api() -> render_api { return s_api; }
    };
}
