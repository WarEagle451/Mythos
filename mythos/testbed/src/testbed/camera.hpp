#pragma once
#include <mythos/core/timestep.hpp>

#include <myl/math/mat4x4.hpp>

#include <myl/math/vec2.hpp>
#include <myl/math/vec3.hpp>

namespace testbed {
    class camera {
        myl::f32vec3 m_position;
        myl::f32vec3 m_rotation;
        myl::f32vec3 m_scale;        

        myl::f32 m_aspect;
        myl::f32 m_fovy;
        myl::f32 m_near;
        myl::f32 m_far;
    public:
        MYL_NO_DISCARD camera(const myl::f32vec3& position, const myl::f32vec3& rotation, const myl::f32vec3& scale, myl::f32 aspect, myl::f32 fovy = 45.f, myl::f32 z_near = .1f, myl::f32 z_far = 1000.f);

        auto projection() const -> myl::f32mat4;
        auto transform() const -> myl::f32mat4;
        auto view_projection() const -> myl::f32mat4 { return projection() * myl::inverse(transform()); }

        auto on_update(myth::timestep ts) -> void;
        auto on_window_resize(const myl::i32vec2& dimensions) -> void;
    };
}
