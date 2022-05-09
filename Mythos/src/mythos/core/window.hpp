#pragma once
#include <mythos/event/event.hpp>
#include <mythos/math/vec2.hpp>

#include <memory>
#include <string>

namespace myl {
	class window {
	public:
		struct config { /// MYTodo: This can also have things like fullscreen, vsync, etc: basically anything glfw would have as window hints
			i32vec2 postion = { 0, 0 };
			i32vec2 size = { 1280, 720 };
			std::string name = "Mythos Application";
		};

		MYL_NO_DISCARD static std::unique_ptr<window> create(const config&);

		virtual ~window() = default;

		//@return A pointer to the instance handle of a window
		MYL_API MYL_NO_DISCARD virtual void* native() const = 0;

		virtual void set_event_callback(const event_callback&) = 0;

		virtual void update() = 0;
	};
}