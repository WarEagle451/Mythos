#pragma once
#include <mythos/platform/window.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace mye::vulkan {
    auto get_platform_required_extensions(std::vector<const char*>* extensions) -> void;

    auto create_surface(VkSurfaceKHR* surface, VkInstance instance, mye::window* target, VkAllocationCallbacks* allocator) -> void;

    MYL_NO_DISCARD auto get_physical_device_presentation_support_khr(VkPhysicalDevice physical_device, uint32_t queue_family_index) -> VkBool32;
}
