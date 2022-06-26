#pragma once
#include <mythos/defines.hpp>

namespace myl::render {
	enum class api {
		none,
		vulkan,
	};

	MYL_NO_DISCARD constexpr const char* api_to_string(api api) {
		switch (api) {
			case api::none: return "None";
			case api::vulkan: return "Vulkan";
			default: return "Unknown";
		}
	}
}