#include <mythos/render/vulkan/vulkan_swapchain.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

#include <myl/algorithm.hpp>

#include <limits>

/// MYTODO: Maybe have a function incontext that is create_swapchain? remove need for context's member getters

namespace myth::vulkan {
    MYL_NO_DISCARD constexpr auto choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) -> VkSurfaceFormatKHR {
        if (available_formats.empty())
            return VkSurfaceFormatKHR{
                .format     = VK_FORMAT_MAX_ENUM,
                .colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR
            };

        for (const auto& format : available_formats)
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && // Preferred format
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;
        return available_formats[0]; /// MYTODO: if the above fails ranking how good formats are to choose would be a good idea
    }

    MYL_NO_DISCARD constexpr auto choose_present_mode(const std::vector<VkPresentModeKHR>& available_modes) -> VkPresentModeKHR {
        for (const auto& mode : available_modes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    MYL_NO_DISCARD constexpr auto choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, window& window) -> VkExtent2D {
        if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            auto fb_size = window.framebuffer_size();
            return VkExtent2D{
                .width  = myl::clamp(static_cast<uint32_t>(fb_size.w), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                .height = myl::clamp(static_cast<uint32_t>(fb_size.h), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
            };
        }
        else
            return capabilities.currentExtent;
    }

    auto swapchain::query_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, swapchain_support_details* details) -> void {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, VK_NULL_HANDLE);
        details->formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details->formats.data());

        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, VK_NULL_HANDLE);
        details->present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details->present_modes.data());
    }

    MYL_NO_DISCARD swapchain::swapchain(context& context, window& window)
        : m_context{ context } {
        create_swapchain(window);
        create_images_and_views();
    }

    swapchain::~swapchain() {
        if (!m_framebuffers.empty())
            destroy_framebuffers();
        destroy_images_and_views();
        destroy_swapchain();
    }

    auto swapchain::create_swapchain(window& window) -> void {
        swapchain_support_details ssd{};
        query_support(m_context.physical_device(), m_context.surface(), &ssd);

        m_surface_format = choose_surface_format(ssd.formats);
        VkPresentModeKHR present_mode = choose_present_mode(ssd.present_modes);
        m_extent = choose_swap_extent(ssd.capabilities, window);

        uint32_t image_count = ssd.capabilities.minImageCount + 1; // Recommended to request one more image than the min
        if (ssd.capabilities.maxImageCount > 0 && image_count > ssd.capabilities.maxImageCount) // Don't excced the max images (0 is a special number)
            image_count = ssd.capabilities.maxImageCount;

        device_queue_indices queue_indices = find_queue_family_indices(m_context.physical_device());
        uint32_t queue_family_indices[] = { queue_indices.graphics, queue_indices.present };

        VkSharingMode sharing_mode{};
        uint32_t queue_family_index_count{};
        uint32_t* pointer_queue_family_indices{};
        if (queue_indices.graphics == queue_indices.present) {
            sharing_mode                 = VK_SHARING_MODE_EXCLUSIVE;
            queue_family_index_count     = 0;
            pointer_queue_family_indices = nullptr;
        }
        else {
            sharing_mode                 = VK_SHARING_MODE_CONCURRENT;
            queue_family_index_count     = 2;
            pointer_queue_family_indices = queue_family_indices;
        }

        VkSwapchainCreateInfoKHR swapchain_create_info{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            ///.pNext = ,
            ///.flags = ,
            .surface               = m_context.surface(),
            .minImageCount         = image_count,
            .imageFormat           = m_surface_format.format,
            .imageColorSpace       = m_surface_format.colorSpace,
            .imageExtent           = m_extent,
            .imageArrayLayers      = 1,
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode      = sharing_mode,
            .queueFamilyIndexCount = queue_family_index_count,
            .pQueueFamilyIndices   = pointer_queue_family_indices,
            .preTransform          = ssd.capabilities.currentTransform,
            .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode           = present_mode,
            .clipped               = VK_TRUE,
            .oldSwapchain          = VK_NULL_HANDLE 
        };

        MYTHOS_VULKAN_VERIFY(vkCreateSwapchainKHR, m_context.device(), &swapchain_create_info, VK_NULL_HANDLE, &m_swapchain);
    }

    auto swapchain::create_images_and_views() -> void {
        uint32_t count{};
        vkGetSwapchainImagesKHR(m_context.device(), m_swapchain, &count, nullptr);
        m_images.resize(count);
        vkGetSwapchainImagesKHR(m_context.device(), m_swapchain, &count, m_images.data());

        m_views.resize(count);
        for (myl::u32 i = 0; i != count; ++i) {
            VkImageViewCreateInfo view_create_info{
                .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                ///.pNext = ,
                ///.flags = ,
                .image            = m_images[i],
                .viewType         = VK_IMAGE_VIEW_TYPE_2D,
                .format           = m_surface_format.format,
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
                },
            };

            MYTHOS_VULKAN_VERIFY(vkCreateImageView, m_context.device(), &view_create_info, VK_NULL_HANDLE, &m_views[i]);
        }
    }

    auto swapchain::recreate_framebuffers(render_pass& render_pass) -> void {
        m_framebuffers.resize(m_views.size());

        for (decltype(m_views)::size_type i = 0; i != m_views.size(); ++i) {
            std::vector<VkImageView> attachments{ m_views[i] };

            VkFramebufferCreateInfo framebuffer_create_info{
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                //.pNext = ,
                //.flags = ,
                .renderPass      = render_pass.handle(),
                .attachmentCount = static_cast<uint32_t>(attachments.size()),
                .pAttachments    = attachments.data(),
                .width           = m_extent.width,
                .height          = m_extent.height,
                .layers          = 1,
            };

            MYTHOS_VULKAN_VERIFY(vkCreateFramebuffer, m_context.device(), &framebuffer_create_info, VK_NULL_HANDLE, &m_framebuffers[i]);
        }
    }

    auto swapchain::destroy_framebuffers() -> void {
        for (auto fb : m_framebuffers)
            vkDestroyFramebuffer(m_context.device(), fb, VK_NULL_HANDLE);
        m_framebuffers.clear();
    }

    auto swapchain::destroy_images_and_views() -> void {
        for (auto& view : m_views)
            vkDestroyImageView(m_context.device(), view, VK_NULL_HANDLE);
        m_views.clear();

        // Images will be destroyed when the swapchain is destroyed and therefore do not need clean up
    }

    auto swapchain::destroy_swapchain() -> void {
        vkDestroySwapchainKHR(m_context.device(), m_swapchain, VK_NULL_HANDLE);
    }
}
