#pragma once
#include <mythos/event/event.hpp>
#include <mythos/math/vec2.hpp>

#include <memory>
#include <string>

namespace myl {
	class window {
	public:
		struct config { /// MYTodo: this can also have things like fullscreen, vsync, etc: basically anything glfw would have as window hints
			i32vec2 postion = { 0, 0 };
			i32 width = 1280;
			i32 height = 720;
			std::string name = "Mythos Application"; // same default in app::info
		};

		MYL_NO_DISCARD static std::unique_ptr<window> create(const config&);

		virtual ~window() = default;

		//@return a pointer to the instance handle of a window
		MYL_API virtual void* native() const = 0;

		virtual void set_event_callback(const event_callback&) = 0;

		virtual void update() = 0;
	};
}