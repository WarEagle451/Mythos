#pragma once
#include <mythos/platform/window.hpp>
#include <mythos/render/vulkan/vulkan_command_buffer.hpp>

#include <vector>

#ifdef MYL_DEBUG /// MYTEMP: MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS should be passed on the command line 
#   define MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

namespace myth::vulkan {
    struct physical_device_requirements {
        bool compute_queue;
        bool graphics_queue;
        bool present_queue;
        bool transfer_queue;

        bool discrete_gpu;

        std::vector<const char*> extensions;
    };

    struct device_queue_indices {
        enum {
            not_available = 65536
        };

        uint32_t compute  = not_available;
        uint32_t graphics = not_available;
        uint32_t present  = not_available;
        uint32_t transfer = not_available;
    };

    MYL_NO_DISCARD constexpr auto find_queue_family_indices(VkPhysicalDevice physical_device) -> device_queue_indices;

    class context {
        VkInstance m_instance;

        VkSurfaceKHR m_surface;

        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        VkQueue m_queue_compute;
        VkQueue m_queue_graphics;
        VkQueue m_queue_present;
        VkQueue m_queue_transfer;

        VkCommandPool m_command_pool;
        std::vector<command_buffer> m_command_buffers;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD context(window& window);
        ~context();

        MYL_NO_DISCARD auto physical_device() const -> VkPhysicalDevice { return m_physical_device; }
        MYL_NO_DISCARD auto device() const -> VkDevice { return m_device; }
        MYL_NO_DISCARD auto surface() const -> VkSurfaceKHR { return m_surface; }
        MYL_NO_DISCARD auto command_pool() const -> VkCommandPool { return m_command_pool; }
        MYL_NO_DISCARD auto command_buffers() -> std::vector<command_buffer>& { return m_command_buffers; }

        MYL_NO_DISCARD auto queue_compute() -> VkQueue { return m_queue_compute; }
        MYL_NO_DISCARD auto queue_graphics() -> VkQueue { return m_queue_graphics; }
        MYL_NO_DISCARD auto queue_present() -> VkQueue { return m_queue_present; }
        MYL_NO_DISCARD auto queue_transfer() -> VkQueue { return m_queue_transfer; }

        auto create_command_buffers(uint32_t count) -> void;
    private:
        auto create_instance() -> void;
        auto create_surface(window& window) -> void;
        auto create_device(device_queue_indices* dqi) -> void;
        auto create_command_pool(const device_queue_indices& dqi) -> void;

        auto destroy_command_objects() -> void;
        auto destroy_device() -> void;
        auto destroy_surface() -> void;
        auto destroy_instance() -> void;
    };
}
