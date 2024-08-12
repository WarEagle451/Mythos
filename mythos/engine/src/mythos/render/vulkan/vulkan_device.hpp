#pragma once
#include <myl/definitions.hpp>

#include <vulkan/vulkan.h>

#include <limits>
#include <vector>

namespace myth::vulkan {
    struct device_queue_indices {
        enum : uint32_t { not_available = std::numeric_limits<uint32_t>::max() };

        union {
            uint32_t values[4]{ not_available, not_available, not_available, not_available };
            struct { uint32_t compute, graphics, present, transfer; };
        };
    };

    struct physical_device_requirements {
        bool discrete_gpu = false;

        bool queue_compute  = false;
        bool queue_graphics = false;
        bool queue_present  = false;
        bool queue_transfer = false;

        std::vector<const char*> extensions;
    };

    class device {
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties m_properties{};
        VkPhysicalDeviceFeatures   m_available_features{};
        VkPhysicalDeviceFeatures   m_enabled_features{ VK_FALSE };
        device_queue_indices       m_qfi{};

        VkQueue m_queue_compute  = VK_NULL_HANDLE;
        VkQueue m_queue_graphics = VK_NULL_HANDLE;
        VkQueue m_queue_present  = VK_NULL_HANDLE;
        VkQueue m_queue_transfer = VK_NULL_HANDLE;
    public:
        struct create_info {
            VkInstance                   instance;
            VkSurfaceKHR                 surface;
            physical_device_requirements physical_device_requirements{};
            VkPhysicalDevice             physical_device = VK_NULL_HANDLE;
        };

        static auto create(device* handle, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(device* handle, VkAllocationCallbacks* allocator) noexcept -> void;

        MYL_NO_DISCARD constexpr device() = default;
        constexpr ~device() = default;

        MYL_NO_DISCARD constexpr auto physical() const noexcept -> VkPhysicalDevice { return m_physical_device; }
        MYL_NO_DISCARD constexpr auto logical() const noexcept -> VkDevice { return m_device; }

        MYL_NO_DISCARD constexpr auto properties() const noexcept -> const VkPhysicalDeviceProperties& { return m_properties; }
        MYL_NO_DISCARD constexpr auto features_available() const noexcept -> const VkPhysicalDeviceFeatures& { return m_available_features; }
        MYL_NO_DISCARD constexpr auto features_enabled() const noexcept -> const VkPhysicalDeviceFeatures& { return m_enabled_features; }

        MYL_NO_DISCARD constexpr auto queue_family_indices() const noexcept -> const device_queue_indices& { return m_qfi; }
        MYL_NO_DISCARD constexpr auto compute_queue() const noexcept -> VkQueue { return m_queue_compute; }
        MYL_NO_DISCARD constexpr auto graphics_queue() const noexcept -> VkQueue { return m_queue_graphics; }
        MYL_NO_DISCARD constexpr auto present_queue() const noexcept -> VkQueue { return m_queue_present; }
        MYL_NO_DISCARD constexpr auto transfer_queue() const noexcept -> VkQueue { return m_queue_transfer; }

        MYL_NO_DISCARD auto find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const -> uint32_t;
    };
}
