#pragma once
#include <mythos/core/timestep.hpp>

/// MYTemp:
#include <mythos/math/matrix.hpp>
#include <mythos/math/projection.hpp>
#include <mythos/math/transform.hpp>

namespace myl {
	class camera { /// MYTemp:
		f32mat4x4 m_view{};

		f32vec3 m_position{ 0, 0, -20 };
		f32vec3 m_rotation{ 0, 0, 0 }; /// MYTodo: Switch to quaternions when you can add their rotations

		u32vec2 m_viewport{ 0, 0 };

		f32 m_fov = 45.f;
		f32 m_aspect = 0.f;
		f32 m_far = 1000.f;
		f32 m_near = .1f;
	public:
		camera() = default;
		virtual ~camera() = default;

		const f32vec3& rotation() const { return m_rotation; }
		const f32mat4x4 projection() const { return perspective(m_fov, m_aspect, m_near, m_far); }
		const f32mat4x4& view() const { return m_view; }

		void recaculate_view() {
			m_view = inverse(translation(m_position) * euler_xyz(m_rotation));
		}

		void update();
	};

	class scene {
		camera m_camera; /// MYTemp:
	public:
		MYL_API scene() = default;
		MYL_API ~scene() = default;

		MYL_API void update(timestep);
		MYL_API void render();
	};
}