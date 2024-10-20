#pragma once
#include <mythos/core/keycodes.hpp>
#include <mythos/core/mousecodes.hpp>
#include <mythos/platform/hid.hpp>

#include <myl/math/vec2.hpp>

#include <array>
#include <memory>
#include <vector>

namespace myth {
    struct keyboard {
        enum class state {
            up = false,
            down = true
        };

        std::array<state, key::size> keys;
    };

    struct mouse {
        mousecode button_states;
        myl::f32vec2 cursor_delta;
        myl::f32vec2 scroll_delta;
        myl::f32vec2 window_cursor_position;
    };

    class input {
        static keyboard                                       s_keyboard;
        static mouse                                          s_mouse;
        static std::vector<std::unique_ptr<hid::device>> s_registered_devices;
    public:
        static auto init() -> void;
        static auto shutdown() -> void;
        static auto update() -> void;
        static auto clear() -> void;

        static MYL_API auto register_device(std::unique_ptr<hid::device>&& new_device) -> bool;
        static MYL_API auto remove_device(hid::device::id_type id) -> bool;
        static MYL_API auto remove_device(hid::device* handle) -> bool;

        static MYL_API auto get_keyboard() -> keyboard& { return s_keyboard; }
        static MYL_API auto get_mouse() -> mouse& { return s_mouse; }
        static MYL_API auto get_device(hid::device::id_type id) -> hid::device*;
        static MYL_API auto registered_devices() -> std::vector<std::unique_ptr<hid::device>>& { return s_registered_devices; }

        // Keyboard Related

        MYL_NO_DISCARD static MYL_API auto key_state(keycode code) noexcept -> keyboard::state { return s_keyboard.keys[code]; }
        MYL_NO_DISCARD static MYL_API auto key_states() noexcept -> const std::array<keyboard::state, key::size>& { return s_keyboard.keys; }
        MYL_NO_DISCARD static MYL_API auto key_up(keycode code) noexcept -> bool { return s_keyboard.keys[code] == keyboard::state::up; }
        MYL_NO_DISCARD static MYL_API auto key_down(keycode code) noexcept -> bool { return s_keyboard.keys[code] == keyboard::state::down; }

        // Mouse Related

        MYL_NO_DISCARD static MYL_API auto mouse_button_states() noexcept -> mousecode { return s_mouse.button_states; }
        MYL_NO_DISCARD static MYL_API auto mouse_buttons_up(mousecode code) noexcept -> bool { return (s_mouse.button_states & code) == code; }
        MYL_NO_DISCARD static MYL_API auto mouse_buttons_down(mousecode code) noexcept -> bool { return (s_mouse.button_states & code) == code; }
        MYL_NO_DISCARD static MYL_API auto cursor_delta() noexcept -> const myl::f32vec2& { return s_mouse.cursor_delta; }
        MYL_NO_DISCARD static MYL_API auto scroll_delta() noexcept -> const myl::f32vec2& { return s_mouse.scroll_delta; }
        
        static auto set_cursor_position(const myl::i32vec2& position) -> void;
        static auto set_cursor_visibility(bool visible) -> void;
        static auto confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void;
        static auto release_cursor() -> void;

        // HID Related

        MYL_NO_DISCARD static MYL_API auto hid_button_states(hid::buttons* device) noexcept -> hid_button_code { return device->button_states; }
        MYL_NO_DISCARD static MYL_API auto hid_buttons_up(hid::buttons* device, hid_button_code code) noexcept -> bool { return (device->button_states & code) == code; }
        MYL_NO_DISCARD static MYL_API auto hid_buttons_down(hid::buttons* device, hid_button_code code) noexcept -> bool { return (device->button_states & code) == code; }

        // Keyboard Processing / Related

        static auto process_key(keycode code, keyboard::state state) -> void;
        static auto process_typed(myl::u16 character) -> void;

        // Mouse Processing Related

        static auto process_mouse_buttons_up(mousecode code) -> void;
        static auto process_mouse_buttons_down(mousecode code) -> void;
        static auto process_mouse_wheel(const myl::f32vec2& delta) -> void;
        static auto process_cursor_delta(const myl::f32vec2& delta) -> void;
        static auto process_window_cursor_position(const myl::f32vec2& position) -> void;

        // HID Processing

        static auto process_hid(hid::device::id_type id, myl::u8* data, myl::u32 byte_count) -> void;
        static auto process_hid_buttons(hid::device* device, hid::buttons* data, hid_button_code down) -> void;
    private:
        static auto query_togglable_keys(keyboard* keyboard) -> void;
    };
}
