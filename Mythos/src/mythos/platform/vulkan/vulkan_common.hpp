#pragma once
#include <mythos/core/assert.hpp>

#include <vulkan/vulkan.h>

#include <format> /// MYTemp: Until variable argument macros are added
#define MYL_VK_CHECK(func, ...) {\
	VkResult a_unique_result_name = func(__VA_ARGS__);\
	MYL_CORE_ASSERT(a_unique_result_name == VK_SUCCESS, std::format("'{}' failed. Result: {}", #func, static_cast<u32>(a_unique_result_name)));\
}