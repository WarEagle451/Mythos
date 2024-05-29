#include <mythos/render/vulkan2/vulkan_device.hpp>
#include <mythos/render/vulkan2/vulkan_platform.hpp>
#include <mythos/render/vulkan2/vulkan_swapchain.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

#include <unordered_map>

namespace myth::vulkan2 {
    MYL_NO_DISCARD static auto get_queue_family_indices(VkPhysicalDevice physical_device) -> device_queue_indices {
        uint32_t queue_family_count{};
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, VK_NULL_HANDLE);
        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties.data());

        device_queue_indices dqi{};
        myl::u8 best_transfer_queue_score = 255; // The queue with the lowest index increases the chance it is a dedicated transfer queue
        for (myl::u32 i = 0; const auto & queue : queue_family_properties) {
            myl::u8 current_transfer_queue_score = 0;

            if (dqi.graphics == device_queue_indices::not_available && queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                dqi.graphics = i;
                ++current_transfer_queue_score;

                // If there is also a present queue, this prioritizes a grouping of 2
                if (get_physical_device_presentation_support_khr(physical_device, i) == VK_TRUE) {
                    dqi.present = i;
                    ++current_transfer_queue_score;
                }
            }

            // Fallback if the graphics queue is not shared with present
            if (dqi.present == device_queue_indices::not_available && get_physical_device_presentation_support_khr(physical_device, i) == VK_TRUE) {
                dqi.present = i;
                ++current_transfer_queue_score;
            }

            if (queue.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                dqi.compute = i;
                ++current_transfer_queue_score;
            }

            if (queue.queueFlags & VK_QUEUE_TRANSFER_BIT && current_transfer_queue_score <= best_transfer_queue_score) {
                dqi.transfer = i;
                best_transfer_queue_score = current_transfer_queue_score;
            }

            ++i;
        }

        return dqi;
    }

    MYL_NO_DISCARD static auto physical_device_meets_requirements(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VkPhysicalDeviceProperties* properties, VkPhysicalDeviceFeatures* features, const physical_device_requirements& requirements) -> bool {
        vkGetPhysicalDeviceProperties(physical_device, properties);
        vkGetPhysicalDeviceFeatures(physical_device, features);

        if (requirements.discrete_gpu && (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU))
            return false;

        // Check queue support

        const device_queue_indices dqi = get_queue_family_indices(physical_device);
        if (requirements.queue_compute  && (dqi.compute == device_queue_indices::not_available)) return false;
        if (requirements.queue_graphics && (dqi.graphics == device_queue_indices::not_available)) return false;
        if (requirements.queue_present  && (dqi.present == device_queue_indices::not_available)) return false;
        if (requirements.queue_transfer && (dqi.transfer == device_queue_indices::not_available)) return false;

        // Check if physical device supports the required extensions

        if (!requirements.extensions.empty()) {
            uint32_t available_extension_count = 0;
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_extension_count, VK_NULL_HANDLE);

            if (available_extension_count == 0)
                return false;

            std::vector<VkExtensionProperties> available_extensions(available_extension_count);
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_extension_count, available_extensions.data());
            for (const auto& r : requirements.extensions) {
                bool found = false;
                for (const auto& a : available_extensions)
                    if (strcmp(r, a.extensionName) == 0) {
                        found = true;
                        break;
                    }

                if (!found)
                    return false;
            }
        }

        // Check the device can support the swapchain

        swapchain_support_details ssd{};
        swapchain::query_support(&ssd, physical_device, surface);
        if (ssd.formats.empty() || ssd.present_modes.empty())
            return false;

        return true;
    }

    MYL_NO_DISCARD static auto rate_physical_device(const VkPhysicalDeviceProperties& properties, const VkPhysicalDeviceFeatures& features) -> myl::u64 {
        myl::u64 rating = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            rating += 1000;

        // Maximum possible size of textures affects graphics quality
        rating += properties.limits.maxImageDimension2D;

        return rating;
    }

    MYL_NO_DISCARD static auto select_physical_device(VkInstance instance, VkSurfaceKHR surface, const physical_device_requirements& requirements) -> VkPhysicalDevice {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance, &count, VK_NULL_HANDLE);
        if (count == 0) {
            MYTHOS_FATAL("Failed to locate a GPU with Vulkan support");
            return VK_NULL_HANDLE;
        }
        
        std::vector<VkPhysicalDevice> canidates(count);
        vkEnumeratePhysicalDevices(instance, &count, canidates.data());
        
        myl::u64 best_rating = 0;
        VkPhysicalDevice best_canidate = VK_NULL_HANDLE;
        for (const auto& canidate : canidates) {
            VkPhysicalDeviceProperties properties{};
            VkPhysicalDeviceFeatures features{};

            if (physical_device_meets_requirements(canidate, surface, &properties, &features, requirements)) {
                // If the device meets requirements its rating is 1
                myl::u64 rating = 1 + rate_physical_device(properties, features);
                if (rating > best_rating) {
                    best_canidate = canidate;
                    best_rating = rating;
                }
            }
        }

        if (best_canidate == VK_NULL_HANDLE)
            MYTHOS_FATAL("No GPUs meet requirements");
        return best_canidate;
    }

    auto device::create(device* h, const create_info& ci) -> void {
        // Select physical device

        h->m_physical_device = ci.physical_device == VK_NULL_HANDLE ? select_physical_device(ci.instance, ci.surface, ci.physical_device_requirements) : ci.physical_device;
        if (h->m_physical_device == VK_NULL_HANDLE) {
            MYTHOS_FATAL("Failed to select a physical device");
            return;
        }

        // Store various properties, features, and limits of the physical device for later use

        vkGetPhysicalDeviceProperties(h->m_physical_device, &h->m_properties);
        vkGetPhysicalDeviceFeatures(h->m_physical_device, &h->m_available_features);

        // Create logical device

        const device_queue_indices qfi = get_queue_family_indices(h->m_physical_device);
        MYTHOS_INFO("Queue family indices - compute: {}, graphics: {}, present: {}, transfer: {}", qfi.compute, qfi.graphics, qfi.present, qfi.transfer);
        if (qfi.graphics != qfi.present)
            MYTHOS_WARN("Selected graphics and present queue fmailies use different indices; {} vs {}", qfi.graphics, qfi.present);

        std::unordered_map<uint32_t, uint32_t> unique_queue_families{}; // Unique queue index, queue fmaily count
        if (qfi.compute  != device_queue_indices::not_available) ++unique_queue_families[qfi.compute];
        if (qfi.graphics != device_queue_indices::not_available) ++unique_queue_families[qfi.graphics];
        if (qfi.present  != device_queue_indices::not_available) ++unique_queue_families[qfi.present];
        if (qfi.transfer != device_queue_indices::not_available) ++unique_queue_families[qfi.transfer];

        uint32_t queue_family_properties_count{};
        vkGetPhysicalDeviceQueueFamilyProperties(h->m_physical_device, &queue_family_properties_count, VK_NULL_HANDLE);
        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_properties_count);
        vkGetPhysicalDeviceQueueFamilyProperties(h->m_physical_device, &queue_family_properties_count, queue_family_properties.data());

        device_queue_indices queue_indices{};
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
        queue_create_infos.reserve(unique_queue_families.size());
        std::vector<float> queue_priorities{};
        queue_priorities.reserve(4); /// MYTEMP: Currently only 1 of each queue type can exist at any given time
        for (const auto& q : unique_queue_families) {
            const uint32_t queue_count = queue_family_properties[q.first].queueCount;
            uint32_t remaining_queue_count = queue_count;

            for (uint32_t i = 0; i != 4; ++i)
                if (q.first == qfi.values[i]) { // Unique queue family index == queue family type index
                    if (queue_priorities.empty()) {
                        if (qfi.values[i] == qfi.compute && qfi.compute != device_queue_indices::not_available)
                            queue_priorities.emplace_back(.7f);
                        if (qfi.values[i] == qfi.graphics && qfi.graphics != device_queue_indices::not_available)
                            queue_priorities.emplace_back(.9f);
                        if (qfi.values[i] == qfi.present && qfi.present != device_queue_indices::not_available)
                            queue_priorities.emplace_back(1.f);
                        if (qfi.values[i] == qfi.transfer && qfi.transfer != device_queue_indices::not_available)
                            queue_priorities.emplace_back(.8f);
                    }

                    queue_indices.values[i] = queue_count - (remaining_queue_count == 0 ? 1 : remaining_queue_count);
                    if (remaining_queue_count != 0)
                        --remaining_queue_count;
                }

            queue_create_infos.emplace_back(VkDeviceQueueCreateInfo{
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                //.pNext            = ,
                //.flags            = ,
                .queueFamilyIndex = q.first,
                .queueCount       = queue_count - remaining_queue_count,
                .pQueuePriorities = queue_priorities.data()
            });

            queue_priorities.clear();
        }

        VkDeviceCreateInfo device_create_info{
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            //.pNext                   = ,
            //.flags                   = ,
            .queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size()),
            .pQueueCreateInfos       = queue_create_infos.data(),
            //.enabledLayerCount       = ,
            //.ppEnabledLayerNames     = ,
            .enabledExtensionCount   = static_cast<uint32_t>(ci.physical_device_requirements.extensions.size()),
            .ppEnabledExtensionNames = ci.physical_device_requirements.extensions.data(),
            /// MYTODO: Enable device features .pEnabledFeatures        = &h->m_enabled_features
        };

        MYTHOS_VULKAN_VERIFY(vkCreateDevice, h->m_physical_device, &device_create_info, VK_NULL_HANDLE, &h->m_device);

        // Create queues

        if (qfi.compute != device_queue_indices::not_available)
            vkGetDeviceQueue(h->m_device, qfi.compute, queue_indices.compute, &h->m_queue_compute);
        if (qfi.graphics != device_queue_indices::not_available)
            vkGetDeviceQueue(h->m_device, qfi.graphics, queue_indices.graphics, &h->m_queue_graphics);
        if (qfi.present != device_queue_indices::not_available)
            vkGetDeviceQueue(h->m_device, qfi.present, queue_indices.present, &h->m_queue_present);
        if (qfi.transfer != device_queue_indices::not_available)
            vkGetDeviceQueue(h->m_device, qfi.transfer, queue_indices.transfer, &h->m_queue_transfer);
    }

    auto device::destroy(device* h) noexcept -> void {
        // Queues are implicitly cleaned up when m_device is destroyed

        if (h->m_device)
            vkDestroyDevice(h->m_device, VK_NULL_HANDLE);

        // m_physical_device will be destroyed upon destruction of the vulkan instance
    }
}
