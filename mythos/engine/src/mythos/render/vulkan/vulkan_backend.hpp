#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_swapchain.hpp>

namespace myth::vulkan {
    class backend : public myth::renderer_backend {
        context m_context;
        swapchain m_swapchain;
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();
    };
}
