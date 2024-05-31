#pragma once
#include <myl/platform.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/window.hpp>

#   include <windows.h>

namespace myth::win {
    class window : public myth::window {
        HINSTANCE m_instance;
        HWND m_handle;

        myl::i32vec2 m_fullscreen_position_cache;
        myl::i32vec2 m_fullscreen_dimension_cache;
    public:
        MYL_NO_DISCARD window(const window_configuration& config);
        ~window();

        MYL_NO_DISCARD constexpr auto instance() const -> HINSTANCE { return m_instance; }
        MYL_NO_DISCARD constexpr auto handle() const -> HWND { return m_handle; }

        MYL_NO_DISCARD auto framebuffer_size() const -> myl::i32vec2 override { return m_dimensions; }

        auto set_title(const char*) -> void override;
        auto set_style(window_style) -> void override;
        auto set_state(window_state) -> void override;
        auto set_position(const myl::i32vec2&) -> void override;
        auto set_dimensions(const myl::i32vec2&) -> void override;

        auto close() -> void override;
        auto restore() -> void override;
        auto update() -> void override;
    private:
        static auto CALLBACK process_message(HWND, UINT, WPARAM, LPARAM) -> LRESULT;
    };
}
#endif
