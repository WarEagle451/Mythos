#pragma once
#include <mythos/core/assert.hpp>

#include <vulkan/vulkan.h>

#include <string>
#include <format>

namespace myl::vulkan {
	//@param extended: Indicates whether to also return an extended result
	//@return The error code and/or extended error message in string form
	std::string VkResult_to_string(VkResult result, bool extended = false);

	bool result_is_success(VkResult);
}

#ifdef MYL_BUILD_DEBUG 
#	define MYL_VK_CHECK(func, ...) { const VkResult y7tp = func(__VA_ARGS__); MYL_CORE_ASSERT(y7tp == VK_SUCCESS, std::format("Vulkan - '{}' failed. Result: {}", #func, myl::vulkan::VkResult_to_string(y7tp))); }
#else
#	define MYL_VK_CHECK(func, ...) func(__VA_ARGS__)
#endif