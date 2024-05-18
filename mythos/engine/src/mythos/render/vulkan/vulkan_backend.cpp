#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD backend::backend(const renderer_configuration& config)
        : m_context(*application::get().main_window())
        , m_swapchain(m_context, *application::get().main_window()) {

    }

    backend::~backend() {

    }
}
