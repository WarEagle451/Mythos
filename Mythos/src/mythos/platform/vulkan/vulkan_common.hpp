#pragma once
#include <mythos/core/assert.hpp>

#include <vulkan/vulkan.h>

#include <format>
#ifdef MYL_BUILD_DEBUG /// MYTemp: Until variable argument assert macros are added
#	define MYL_VK_CHECK(func, ...) {\
		const VkResult a_unique_result_name = func(__VA_ARGS__);\
		MYL_CORE_ASSERT(a_unique_result_name == VK_SUCCESS, std::format("Vulkan - '{}' failed. Result: {}", #func, static_cast<u32>(a_unique_result_name)));\
	}
#else
#	define MYL_VK_CHECK(func, ...) func(__VA_ARGS__)
#endif