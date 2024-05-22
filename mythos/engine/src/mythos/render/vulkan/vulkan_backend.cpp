#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>

/// MYTODO: Instead of calling code like below have a second struct that. Call is renderer_create_info

namespace myth::vulkan {
    MYL_NO_DISCARD backend::backend(const renderer_configuration& config)
        : m_context(*application::get().main_window())
        , m_swapchain(m_context, *application::get().main_window()) {

    }

    backend::~backend() {

    }

    auto backend::on_window_resize(const myl::i32vec2& dimensions) -> void {
        ///m_swapchain.recreate(dimensions);
    }
}
