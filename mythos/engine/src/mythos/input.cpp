#include <mythos/event/hid_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/input.hpp>
#include <mythos/log.hpp>

#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/windows/win_input.hpp>
#endif

namespace myth {
    keyboard                          input::s_keyboard;
    mouse                             input::s_mouse;
    std::vector<std::unique_ptr<hid>> input::s_registered_devices = {};

    auto input::init() -> void {
        clear();
    }

    auto input::shutdown() -> void {
        while (!s_registered_devices.empty())
            remove_device(s_registered_devices.back()->id);
    }

    auto input::update() -> void {
        // Deltas need to be reset as it is possible in the next data set that they will be the same as before

        s_mouse.cursor_delta = { 0.f, 0.f };
        s_mouse.scroll_delta = { 0.f, 0.f };

        for (auto& device : s_registered_devices)
            device->update();
    }

    auto input::clear() -> void {
        s_keyboard.keys.fill(keyboard::state::up);        
        query_togglable_keys(&s_keyboard);

        s_mouse.button_states = mouse_button::none;
        s_mouse.cursor_delta = { 0.f, 0.f };
        s_mouse.scroll_delta = { 0.f, 0.f };
        s_mouse.window_cursor_position = { 0.f, 0.f };

        for (auto& device : s_registered_devices)
            device->button_states = hid_button::none;

        /// MYTODO: Clearing state of devices
    }

    auto input::register_device(std::unique_ptr<hid>&& new_device) -> bool {
        for (auto& existing_device : s_registered_devices)
            if (existing_device->id == new_device->id) {
                MYTHOS_WARN("A device with id '{}' is already registered", new_device->id);
                return false;
            }

        s_registered_devices.emplace_back(std::move(new_device));
        MYTHOS_DEBUG("New device registered. ID: {}", s_registered_devices.back()->id);
        return true;
    }

    auto input::remove_device(hid::id_type id) -> bool {
        for (auto it = s_registered_devices.begin(), end = s_registered_devices.end(); it != end; ++it)
            if ((*it)->id == id) {
                s_registered_devices.erase(it);
                MYTHOS_DEBUG("Device removed. ID: {}", id);
                return true;
            }

        MYTHOS_WARN("A device with id '{}' is not registered", id);
        return false;
    }

    auto input::remove_device(hid* handle) -> bool {
        return remove_device(handle->id);
    }

    auto input::get_device(hid::id_type id) -> hid* {
        for (auto& existing_device : s_registered_devices)
            if (existing_device->id == id)
                return existing_device.get();

        MYTHOS_WARN("A device with id '{}' is not registered", id);
        return nullptr;
    }

    auto input::set_cursor_position(const myl::i32vec2& position) -> void {
#ifdef MYL_OS_WINDOWS
        win::set_cursor_position(position);
#else
        MYTHOS_WARN("myth::input::set_cursor_position is not implemented on this platform");
#endif
    }

    auto input::set_cursor_visibility(bool visible) -> void {
#ifdef MYL_OS_WINDOWS
        win::set_cursor_visibility(visible);
#else
        MYTHOS_WARN("myth::input::set_cursor_visability is not implemented on this platform");
#endif
    }

    auto input::confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void {
#ifdef MYL_OS_WINDOWS
        win::confine_cursor(tl, br);
#else
        MYTHOS_ERROR("myth::input::confine_cursor is not implemented on this platform");
#endif
    }

    auto input::release_cursor() -> void {
#ifdef MYL_OS_WINDOWS
        win::release_cursor();
#else
        MYTHOS_ERROR("myth::input::unconfine_cursor is not implemented on this platform");
#endif
    }

    auto input::process_key(keycode code, keyboard::state state) -> void {
        if (code == key::unknown)
            return;

        if (s_keyboard.keys[code] != state) { // Pressed (Non held) and released events should only be fired on key state change
            s_keyboard.keys[code] = state;

            if (state == keyboard::state::up) {
                event::key_released e(code);
                event::fire(e);
            }
            else {
                event::key_pressed e(code, false);
                event::fire(e);
            }
        }
        else if (state == keyboard::state::down) { // Key is held down
            event::key_pressed e(code, true);
            event::fire(e);
        }
    }

    auto input::process_typed(myl::u16 character) -> void {
        /// MYTODO: Allow a wider ranger of characters
        if (character < 32 || (character > 126 && character < 160))
            return;
        event::typed e(character);
        event::fire(e);
    }

    auto input::process_mouse_buttons_up(mousecode code) -> void {
        // new = 0110; old = 1010
        // old & new = 0010. Therefore bit 2 has changed state to up
        const mousecode changed_buttons = s_mouse.button_states & code;
        if (changed_buttons != 0) {
            s_mouse.button_states &= ~changed_buttons; // Clear bits
            event::mouse_released e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_buttons_down(mousecode code) -> void {
        // new = 0110; old = 1010
        // ~(old | ~new) = 0100. Therefore bit 3 has changed state to down
        const mousecode changed_buttons = ~(s_mouse.button_states | ~code);
        if (changed_buttons != 0) {
            s_mouse.button_states |= changed_buttons; // Set bits
            event::mouse_pressed e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_wheel(const myl::f32vec2& delta) -> void {
        s_mouse.scroll_delta = delta;
        event::mouse_scrolled e(delta);
        event::fire(e);
    }

    auto input::process_cursor_delta(const myl::f32vec2& delta) -> void {
        if (delta.x != 0.f || delta.y != 0.f)
            s_mouse.cursor_delta = delta;
    }

    auto input::process_window_cursor_position(const myl::f32vec2& position) -> void {
        if (s_mouse.window_cursor_position != position) {
            s_mouse.window_cursor_position = position;
            event::mouse_moved e(position);
            event::fire(e);
        }
    }

    auto input::process_hid(hid::id_type id, myl::u8* data, myl::u32 byte_count) -> void {
        for (auto& device : s_registered_devices)
            if (device->id == id) {
                if (device->process_callback)
                    device->process_callback(device.get(), data, byte_count);
                return;
            }
    }

    auto input::process_hid_buttons(hid* device, hid_button_code down) -> void {
        // Refer to process_mouse_buttons_down for explanation
        const hid_button_code changed_to_down = ~(device->button_states | ~down);
        if (changed_to_down != hid_button::none) {
            device->button_states |= changed_to_down;
            event::hid_button_pressed e(device, changed_to_down);
            event::fire(e);
        }

        // Refer to process_mouse_buttons_up for explanation
        const hid_button_code changed_to_up = device->button_states & ~down; // ~down == up buttons set to 1
        if (changed_to_up != hid_button::none) {
            device->button_states &= ~changed_to_up;
            event::hid_button_released e(device, changed_to_up);
            event::fire(e);
        }
    }

    auto input::query_togglable_keys(keyboard* keyboard) -> void {
#ifdef MYL_OS_WINDOWS
        win::query_togglable_keys(keyboard);
#else
        MYTHOS_WARN("input::query_togglable_keys is not implemented on this platform");
#endif
    }
}
