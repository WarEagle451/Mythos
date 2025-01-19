#include <mythos/event/device_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/input.hpp>
#include <mythos/log.hpp>

#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/windows/win_input.hpp>
#endif

namespace mye {
    keyboard_state      input::s_keyboard;
    mouse_state         input::s_mouse;
    std::vector<device> input::s_registered_devices;

    auto input::init() -> void {
        clear();
    }

    auto input::shutdown() -> void {
        while (!s_registered_devices.empty())
            remove_device(s_registered_devices.back().handle);
    }

    auto input::update() -> void {
        // Deltas need to be reset as it is possible in the next data set that they will be the same as before

        s_mouse.cursor_delta = { 0.f, 0.f };
        s_mouse.scroll_delta = { 0.f, 0.f };
    }

    auto input::clear() -> void {
        s_keyboard.keys.fill(key_state::up);
        query_togglable_keys(&s_keyboard);

        s_mouse.buttons = mouse_button::none;
        s_mouse.cursor_delta = { 0.f, 0.f };
        s_mouse.scroll_delta = { 0.f, 0.f };
        s_mouse.position = { 0.f, 0.f };
    }

    auto input::register_device(native_device_handle_type handle) -> bool {
        for (auto& existing_device : s_registered_devices)
            if (existing_device.handle == handle) {
                MYTHOS_WARN("A device with handle '{}' is already registered", reinterpret_cast<uintptr_t>(handle));
                return false;
            }

        device new_device{ .handle = handle };
        if (device::create(&new_device)) {
            s_registered_devices.emplace_back(std::move(new_device));
            MYTHOS_DEBUG("New device registered. Handle: {}", reinterpret_cast<uintptr_t>(handle));
            return true;
        }
        else {
            MYTHOS_ERROR("Failed to register new device. Handle: {}", reinterpret_cast<uintptr_t>(handle));
            return false;
        }
    }

    auto input::remove_device(native_device_handle_type handle) -> bool {
        for (auto it = s_registered_devices.begin(), end = s_registered_devices.end(); it != end; ++it)
            if (it->handle == handle) {
                device::destroy(&*it);
                s_registered_devices.erase(it);
                MYTHOS_DEBUG("Device removed. Handle: {}", reinterpret_cast<uintptr_t>(handle));
                return true;
            }

        MYTHOS_WARN("A device with handle '{}' is not registered", reinterpret_cast<uintptr_t>(handle));
        return false;
    }

    auto input::remove_device(device* device) -> bool {
        return remove_device(device->handle);
    }

    auto input::get_device(native_device_handle_type handle) -> device* {
        for (auto& device : s_registered_devices)
            if (device.handle == handle)
                return &device;

        MYTHOS_WARN("A device with id '{}' is not registered", reinterpret_cast<uintptr_t>(handle));
        return nullptr;
    }

    auto input::is_device_registered(native_device_handle_type handle) -> bool {
        for (auto& device : s_registered_devices)
            if (device.handle == handle)
                return true;
        return false;
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

    auto input::process_key(keycode key, mye::key_state state) -> void {
        if (key == key::unknown)
            return;

        if (s_keyboard.keys[key] != state) { // Pressed (Non held) and released events should only be fired on key state change
            s_keyboard.keys[key] = state;

            if (state == key_state::up) {
                event::key_released e(key);
                event::fire(e);
            }
            else {
                event::key_pressed e(key, false);
                event::fire(e);
            }
        }
        else if (state == key_state::down) { // Key is held down
            event::key_pressed e(key, true);
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

    auto input::process_mouse_button_down(const mousecode down) -> void {
        // new = 0110; old = 1010
        // ~(old | ~new) = 0100. Therefore bit 3 has changed state to down
        const mousecode changed_buttons = ~(s_mouse.buttons | ~down);
        if (changed_buttons != mouse_button::none) {
            s_mouse.buttons |= changed_buttons; // Set bits
            event::mouse_pressed e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_button_up(const mousecode up) -> void {
        // new = 0110; old = 1010
        // old & new = 0010. Therefore bit 2 has changed state to up
        const mousecode changed_buttons = s_mouse.buttons & up;
        if (changed_buttons != mouse_button::none) {
            s_mouse.buttons &= ~changed_buttons; // Clear bits
            event::mouse_released e(changed_buttons);
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

    auto input::process_cursor_position(const myl::f32vec2& position) -> void {
        if (s_mouse.position != position) {
            s_mouse.position = position;
            event::mouse_moved e(position);
            event::fire(e);
        }
    }

    auto input::process_device_buttons(device* device, buttons* buttons, const button_code down) -> void {
        // Refer to process_mouse_buttons_down for explanation
        const button_code changed_to_down = ~(*buttons | ~down);
        if (changed_to_down != button::none) {
            *buttons |= changed_to_down;
            event::device_button_pressed e(device, changed_to_down);
            event::fire(e);
        }

        // Refer to process_mouse_buttons_up for explanation
        const button_code changed_to_up = *buttons & ~down;
        if (changed_to_up != button::none) {
            *buttons &= ~changed_to_up;
            event::device_button_released e(device, changed_to_up);
            event::fire(e);
        }
    }

    auto input::query_togglable_keys(mye::keyboard_state* keyboard) -> void {
#ifdef MYL_OS_WINDOWS
        win::query_togglable_keys(keyboard);
#else
        MYTHOS_WARN("input::query_togglable_keys is not implemented on this platform");
#endif
    }
}
