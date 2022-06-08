#pragma once
#include "common.hpp"
#include "shader.hpp"

#include <mythos/core/app.hpp>
#include <mythos/math/mat4x4.hpp>
#include <mythos/math/vec3.hpp>

#include <memory>

namespace myl::render {
	class backend {
	public:
		static MYL_NO_DISCARD std::unique_ptr<backend> create(api);

		virtual ~backend() = default;

		virtual bool begin() = 0;
		virtual void update_global_state(f32mat4x4 a_projection, f32mat4x4 a_view, f32vec3 a_position, f32vec4 a_color, i32 a_mode) = 0; /// MYTodo: I do not like this design, ref to other vulkan tutorial
		virtual void update_object(f32mat4x4 a_model) = 0; /// MYTodo: Get rid of this
		virtual void end() = 0;

		virtual void on_window_resize(const u32vec2&) = 0;

		virtual std::shared_ptr<shader> create_shader(const std::filesystem::path&) = 0;
	};
}