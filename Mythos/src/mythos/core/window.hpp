#pragma once
#include <mythos/event/event.hpp>

#include <memory>
#include <string>
#include <functional>

namespace myl {
	class window {
	public:
		using event_callback = std::function<void(event&)>;

		struct config { /// MYTodo: this can also have things like fullscreen, vsync, etc: basically anything glfw would have as window hints
			i32 x, y;
			i32 width, height;
			std::string name;
		};

		static std::unique_ptr<window> create(const config&);

		virtual ~window() = default;

		MYL_API virtual void* native() const = 0;

		virtual void set_event_callback(const event_callback&) = 0;

		virtual void update() = 0;
	};
}