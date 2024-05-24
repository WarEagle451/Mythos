#pragma once
#include <mythos/render/vulkan/vulkan_context.hpp>

namespace myth::vulkan {
    class pipeline {
        context& m_context;

        VkPipeline m_pipeline;
    public:
        MYL_NO_DISCARD pipeline(context& context);
        ~pipeline();
    };
}
