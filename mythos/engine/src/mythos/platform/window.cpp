#include <mythos/platform/window.hpp>
#include <mythos/input.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/windows/win_window.hpp>
#else
#   include <mythos/log.hpp>
#endif

#include <utility>

namespace myth {
    MYL_NO_DISCARD auto window::create(const window_configuration& config) -> std::unique_ptr<window> {
#ifdef MYL_OS_WINDOWS
        std::unique_ptr<window> window = std::make_unique<myth::win::window>(config);
#else
        MYTHOS_ERROR("Mythos doesn't support window creation on this platform! Returning a nullptr.");
        return nullptr;
#endif
        if (window->state() != window_state::minimized) {
            if (config.center_cursor)
                input::set_cursor_position(window->position() + window->dimensions() / 2);
        }

        return std::move(window);
    }
}
