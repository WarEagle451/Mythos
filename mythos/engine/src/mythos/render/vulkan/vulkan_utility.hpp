#pragma once
#include <mythos/log.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace myth::vulkan {
    MYL_API auto get_platform_required_extensions(std::vector<const char*>* extensions) -> void;

    MYL_NO_DISCARD constexpr MYL_API auto vkresult_string(VkResult code, bool extended_information = false) -> const char*;
}

#define MYTHOS_VULKAN_MAKE_ASSERT(log, extended, func, ...) {\
    const VkResult result = func(__VA_ARGS__);\
    if (result != VK_SUCCESS)\
        log("Vulkan - '"#func"' failed! - {}", myth::vulkan::vkresult_string(result, extended));\
}

#ifdef MYL_DEBUG
#   define MYTHOS_VULKAN_ASSERT(func, ...) MYTHOS_VULKAN_MAKE_ASSERT(MYTHOS_FATAL, true, func, __VA_ARGS__)
#else
#   define MYTHOS_VULKAN_ASSERT(func, ...)
#endif

#define MYTHOS_VULKAN_VERIFY(func, ...) MYTHOS_VULKAN_MAKE_ASSERT(MYTHOS_FATAL, true, func, __VA_ARGS__)