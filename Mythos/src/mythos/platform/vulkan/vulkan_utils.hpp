#pragma once
#include <mythos/core/assert.hpp>

#include <vulkan/vulkan.h>

#include <string>
#include <format> /// MYTemp: Until variable argument assert macros are added

namespace myl::vulkan {
	//@param extended: Indicates whether to also return an extended result
	//@return The error code and/or extended error message in string form
	std::string VkResult_to_string(VkResult result, bool extended = false);
	bool result_is_success(VkResult);
}

#ifdef MYL_BUILD_DEBUG /// MYTemp: Until variable argument assert macros are added
#	define MYL_VK_CHECK(func, ...) {\
		const VkResult a_unique_result_name = func(__VA_ARGS__);\
		MYL_CORE_ASSERT(a_unique_result_name == VK_SUCCESS, std::format("Vulkan - '{}' failed. Result: {}", #func, myl::vulkan::VkResult_to_string(a_unique_result_name)));\
	}
#else
#	define MYL_VK_CHECK(func, ...) func(__VA_ARGS__)
#endif