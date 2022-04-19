#pragma once
#include <mythos/defines.hpp>

#include <memory>
#include <string>

namespace myl {
	class window {
	public:
		struct config {
			std::string name;
			i32 x, y;
			i32 width, height;
		};

		MYL_API static std::unique_ptr<window> create(const config&);

		MYL_API virtual ~window() = default;

		MYL_API virtual void* native() const = 0;

		MYL_API virtual void update() = 0;
	};
}