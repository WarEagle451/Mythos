#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>

namespace myth::vulkan {
    class backend : public myth::renderer_backend {
        vulkan::context m_context;
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();
    };
}
