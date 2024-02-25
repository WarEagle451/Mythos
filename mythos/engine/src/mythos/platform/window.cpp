#include <mythos/platform/window.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/windows/win_window.hpp>
namespace myth {
    MYL_NO_DISCARD auto window::create(const window_configuration& config) -> std::unique_ptr<window> {
        return std::make_unique<myth::win::window>(config);
    }
}
#else
#   include <mythos/log.hpp>
namespace myth {
    MYL_NO_DISCARD auto window::create(const window_configuration& config) -> std::unique_ptr<window> {
        MYTHOS_ERROR("Mythos doesn't support window creation on this platform! Returning a nullptr.");
        return nullptr;
    }
}
#endif
