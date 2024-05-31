#include <mythos/render/vulkan2/vulkan_swapchain.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

#include <myl/algorithm.hpp>

#include <limits>

namespace myth::vulkan2 {
    MYL_NO_DISCARD auto select_image_extent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& framebuffer_extent) -> VkExtent2D {
        return capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max() ?
            VkExtent2D{
                .width  = myl::clamp(framebuffer_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                .height = myl::clamp(framebuffer_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
            } :
            capabilities.currentExtent;
    }

    MYL_NO_DISCARD constexpr auto select_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) -> VkSurfaceFormatKHR {
        if (available_formats.empty())
            return VkSurfaceFormatKHR{
                .format     = VK_FORMAT_MAX_ENUM,
                .colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR
            };
        
        for (const auto& format : available_formats)
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && // Preferred format
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;

        // If none of the formats are the preferred format use the first in the list
        return available_formats[0];
    }

    MYL_NO_DISCARD constexpr auto select_present_mode(const std::vector<VkPresentModeKHR>& available_modes) -> VkPresentModeKHR {
        for (const auto& mode : available_modes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    auto swapchain::query_support(swapchain_support_details* details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) -> void {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

        uint32_t format_count{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, VK_NULL_HANDLE);
        if (format_count != 0) {
            details->available_surface_formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details->available_surface_formats.data());
        }

        uint32_t present_mode_count{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, VK_NULL_HANDLE);
        if (present_mode_count != 0) {
            details->available_present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details->available_present_modes.data());
        }
    }

    auto swapchain::create(swapchain* h, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        // Create swapchain

        swapchain_support_details ssd{};
        query_support(&ssd, device.physical(), ci.surface);

        h->m_extent = select_image_extent(ssd.capabilities, ci.extent);
        h->m_image_format = select_surface_format(ssd.available_surface_formats);
        const VkPresentModeKHR present_mode = select_present_mode(ssd.available_present_modes);
                
        uint32_t image_count = ssd.capabilities.minImageCount + 1;                              // Recommended to request one more image than the min
        if (ssd.capabilities.maxImageCount > 0 && image_count > ssd.capabilities.maxImageCount) // Don't excced the max images (0 is a special number)
            image_count = ssd.capabilities.maxImageCount;
        h->m_max_frames_in_flight = image_count;

        const device_queue_indices& qfi = device.queue_family_indices();
        uint32_t queue_family_indices[] = {
            qfi.graphics,
            qfi.present
        };

        VkSharingMode sharing_mode{};
        uint32_t queue_families_count{};
        uint32_t* queue_family_indices_ptr{};
        if (qfi.graphics == qfi.present) {
            sharing_mode             = VK_SHARING_MODE_EXCLUSIVE;
            queue_families_count     = 0;
            queue_family_indices_ptr = nullptr;
        }
        else {
            sharing_mode             = VK_SHARING_MODE_CONCURRENT;
            queue_families_count     = 2;
            queue_family_indices_ptr = queue_family_indices;
        }

        VkSwapchainCreateInfoKHR swapchain_create_info{
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            //.pNext = ,
            //.flags = ,
            .surface               = ci.surface,
            .minImageCount         = image_count,
            .imageFormat           = h->m_image_format.format,
            .imageColorSpace       = h->m_image_format.colorSpace,
            .imageExtent           = h->m_extent,
            .imageArrayLayers      = 1,
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode      = sharing_mode,
            .queueFamilyIndexCount = queue_families_count,
            .pQueueFamilyIndices   = queue_family_indices_ptr,
            .preTransform          = ssd.capabilities.currentTransform,
            .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode           = present_mode,
            .clipped               = VK_TRUE,
            .oldSwapchain          = VK_NULL_HANDLE
        };

        MYTHOS_VULKAN_VERIFY(vkCreateSwapchainKHR, device.logical(), &swapchain_create_info, allocator, &h->m_swapchain);

        // Create image resources

        vkGetSwapchainImagesKHR(device.logical(), h->m_swapchain, &image_count, VK_NULL_HANDLE);
        h->m_images.resize(image_count);
        vkGetSwapchainImagesKHR(device.logical(), h->m_swapchain, &image_count, h->m_images.data());

        h->m_views.resize(image_count);
        for (uint32_t i = 0; i != image_count; ++i) {
            VkImageViewCreateInfo view_create_info{
                .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                ///.pNext = ,
                ///.flags = ,
                .image            = h->m_images[i],
                .viewType         = VK_IMAGE_VIEW_TYPE_2D,
                .format           = h->m_image_format.format,
                .components       = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1
                }
            };

            MYTHOS_VULKAN_VERIFY(vkCreateImageView, device.logical(), &view_create_info, allocator, &h->m_views[i]);
        }

        // Create sync objects
    }

    auto swapchain::destroy(swapchain* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        // Destroy sync objects
        
        // Destroy framebuffers

        h->destroy_framebuffers(device, allocator);

        // Destroy image views

        for (auto& view : h->m_views)
            vkDestroyImageView(device.logical(), view, allocator);
        h->m_views.clear();

        // Images are cleaned when the swapchain is destroyed

        // Destroy swapchain

        if (h->m_swapchain)
            vkDestroySwapchainKHR(device.logical(), h->m_swapchain, allocator);
    }

    auto swapchain::recreate_framebuffers(device& device, VkRenderPass render_pass, VkAllocationCallbacks* allocator) -> void {
        m_framebuffers.resize(m_views.size());

        for (decltype(m_views)::size_type i = 0; i != m_views.size(); ++i) {
            std::vector<VkImageView> attachments{ m_views[i] };

            VkFramebufferCreateInfo framebuffer_create_info{
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                //.pNext = ,
                //.flags = ,
                .renderPass      = render_pass,
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments    = attachments.data(),
                .width           = m_extent.width,
                .height          = m_extent.height,
                .layers          = 1
            };

            MYTHOS_VULKAN_VERIFY(vkCreateFramebuffer, device.logical(), &framebuffer_create_info, allocator, &m_framebuffers[i]);
        }
    }

    auto swapchain::destroy_framebuffers(device& device, VkAllocationCallbacks* allocator) -> void {
        for (auto& fb : m_framebuffers)
            vkDestroyFramebuffer(device.logical(), fb, allocator);
        m_framebuffers.clear();
    }
}
