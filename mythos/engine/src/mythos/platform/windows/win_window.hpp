#pragma once
#include <myl/platform.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/window.hpp>

#	include <windows.h>

namespace myth::win {
    class window : public myth::window {
        HINSTANCE m_instance;
        HWND m_handle;
    public:
        MYL_NO_DISCARD window(const window_configuration& config);
        ~window();

        auto update() -> void;
    private:
        static auto CALLBACK process_message(HWND, UINT, WPARAM, LPARAM) -> LRESULT;
    };
}
#endif
