#pragma once
#include <mythos/event/event.hpp>
#include <mythos/math/vec2.hpp>

#include <memory>
#include <string>

namespace myl {
	class window {
	public:
		struct config {
			i32vec2 postion = { 0, 0 };
			i32vec2 size = { 1280, 720 };
			std::string name = "Mythos Application";
		};

		MYL_NO_DISCARD static std::unique_ptr<window> create(const config&);

		virtual ~window() = default;

		//@return A pointer to the instance handle of a window
		MYL_API MYL_NO_DISCARD virtual void* native() const = 0;

		virtual void update() = 0;
	};
}