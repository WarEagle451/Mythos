#pragma once
#include <mythos/input/device.hpp>

#include <vector>

namespace mye {
    class input {
        static keyboard_state      s_keyboard;
        static mouse_state         s_mouse;
        static std::vector<device> s_registered_devices;
    public:
        static auto init() -> void;
        static auto shutdown() -> void;
        static auto update() -> void;
        static auto clear() -> void;

        static auto register_device(native_device_handle_type handle) -> bool;
        static auto remove_device(native_device_handle_type handle) -> bool;
        static auto remove_device(device* device) -> bool;

        // Getters

        MYL_NO_DISCARD static MYL_API auto keyboard_state() -> const keyboard_state { return s_keyboard; }
        MYL_NO_DISCARD static MYL_API auto mouse_state() -> const mouse_state { return s_mouse; }
        MYL_NO_DISCARD static MYL_API auto registered_devices() -> std::vector<device>& { return s_registered_devices; }
        MYL_NO_DISCARD static MYL_API auto get_device(native_device_handle_type handle) -> device*;

        // Query

        MYL_NO_DISCARD static MYL_API auto is_device_registered(native_device_handle_type handle) -> bool;

        MYL_NO_DISCARD static MYL_API auto key_state(keycode code) noexcept -> key_state { return s_keyboard.keys[code]; }
        MYL_NO_DISCARD static MYL_API auto key_states() noexcept -> const mye::keyboard_state& { return s_keyboard; }
        MYL_NO_DISCARD static MYL_API auto key_up(keycode code) noexcept -> bool { return s_keyboard.keys[code] == key_state::up; }
        MYL_NO_DISCARD static MYL_API auto key_down(keycode code) noexcept -> bool { return s_keyboard.keys[code] == key_state::down; }

        MYL_NO_DISCARD static MYL_API auto mouse_button_states() noexcept -> mousecode { return s_mouse.buttons; }
        MYL_NO_DISCARD static MYL_API auto mouse_buttons_up(mousecode code) noexcept -> bool { return (s_mouse.buttons & code) == code; }
        MYL_NO_DISCARD static MYL_API auto mouse_buttons_down(mousecode code) noexcept -> bool { return (s_mouse.buttons & code) == code; }
        MYL_NO_DISCARD static MYL_API auto cursor_delta() noexcept -> const myl::f32vec2& { return s_mouse.cursor_delta; }
        MYL_NO_DISCARD static MYL_API auto scroll_delta() noexcept -> const myl::f32vec2& { return s_mouse.scroll_delta; }

        // Action

        static auto set_cursor_position(const myl::i32vec2& position) -> void;
        static auto set_cursor_visibility(bool visible) -> void;
        static auto confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void;
        static auto release_cursor() -> void;

        // Input processing

        static auto process_key(keycode key, mye::key_state state) -> void;
        static auto process_typed(myl::u16 character) -> void;

        static auto process_mouse_button_down(const mousecode down) -> void;
        static auto process_mouse_button_up(const mousecode up) -> void;
        static auto process_mouse_wheel(const myl::f32vec2& delta) -> void;
        static auto process_cursor_delta(const myl::f32vec2& delta) -> void;
        static auto process_cursor_position(const myl::f32vec2& position) -> void;

        static auto process_device_buttons(device* device, buttons* buttons, const button_code down) -> void;
    private:
        static auto query_togglable_keys(mye::keyboard_state* keyboard) -> void;
    };
}
