#pragma once
#include <mythos/math/mat4x4.hpp>

namespace myl::render { /// MYTodo: Hate this design
	struct global_unifrom_object {
		f32mat4x4 projection;	// 64 bytes
		f32mat4x4 view;			// 64 bytes
		f32mat4x4 reserved0;	// 64 bytes, reserved for future use
		f32mat4x4 reserved1;	// 64 bytes, reserved for future use
	};
}

// Some NVidia cards require uniforms to be 256 bytes
static_assert(sizeof(myl::render::global_unifrom_object) == 256, "Expected global_unifrom_object to be 256 bytes");