#pragma once
#include <mythos/render/renderer_backend.hpp>

namespace myth::vulkan {
    class backend : public myth::renderer_backend {
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();
    };
}
