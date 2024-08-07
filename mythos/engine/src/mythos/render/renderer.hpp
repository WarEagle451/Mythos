#pragma once
#include <mythos/render/renderer_backend.hpp>

namespace myth {
    class renderer {
        static render_api s_api;
        static std::unique_ptr<renderer_backend> s_backend;
    public:
        static auto init(const renderer_configuration& config) -> void;
        static auto shutdown() -> void;

        MYL_NO_DISCARD static MYL_API auto api() -> render_api { return s_api; }
        static auto backend() noexcept -> renderer_backend* { return s_backend.get(); }

        static auto set_clear_color(const myl::f32vec3& color) -> void;
        static auto set_vsync(bool value) -> void;

        static auto begin_frame() -> bool;
        static auto end_frame() -> void;

        static auto draw() -> void; /// MYTEMP: Replace with a proper draw command
    };
}
