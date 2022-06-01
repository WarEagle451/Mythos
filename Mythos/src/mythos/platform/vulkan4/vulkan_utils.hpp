#pragma once
#include <mythos/core/assert.hpp>
#include <mythos/core/except.hpp>

#include <vulkan/vulkan.h>

#include <string>
#include <format>

namespace myl::vulkan4 {
	//@param extended: Indicates whether to also return an extended result
	//@return The error code and/or extended error message in string form
	std::string VkResult_to_string(VkResult, bool extended = false);

	bool result_is_success(VkResult);

	struct vulkan_error : public myl::core_exception {
		explicit vulkan_error(std::string_view a_message)
			: myl::core_exception("Vulkan - ", a_message) {}
	};
}

#define MYL_VK_VERIFY(func, ...) {\
	const VkResult t5p9 = func(__VA_ARGS__);\
	if (t5p9 != VK_SUCCESS) {\
		std::string t5p9_string("'"#func"' failed. Result: ");\
		t5p9_string.append(VkResult_to_string(t5p9));\
		throw myl::vulkan4::vulkan_error(t5p9_string);\
	}\
}

#ifdef MYL_BUILD_DEBUG 
#	define MYL_VK_ASSERT(func, ...) {\
		const VkResult t5p9 = func(__VA_ARGS__);\
		MYL_CORE_ASSERT(t5p9 == VK_SUCCESS, std::format("Vulkan - '{}' failed. Result: {}", #func, myl::vulkan4::VkResult_to_string(t5p9)));\
	}
#else
#	define MYL_VK_ASSERT(func, ...) func(__VA_ARGS__)
#endif