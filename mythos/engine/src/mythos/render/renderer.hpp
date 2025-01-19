#pragma once
#include <mythos/render/renderer_backend.hpp>

#include <myl/math/mat4x4.hpp>
#include <myl/math/vec3.hpp>
#include <myl/math/vec4.hpp>

namespace mye {
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

        static auto begin_quad_batch() -> void;
        static auto begin_batch() -> void;

        static auto flush_quad_batch() -> void;
        static auto flush_batch() -> void;

        static auto next_quad_batch() -> void;
        static auto next_batch() -> void;

        static auto begin(const myl::f32mat4x4& view_projection) -> void;
        static auto end() -> void;

        static auto draw_quad(const myl::f32vec3& pos, const myl::f32vec4& color) -> void;
    };
}
