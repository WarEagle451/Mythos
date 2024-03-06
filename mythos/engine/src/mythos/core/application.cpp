#include <mythos/assert.hpp>
#include <mythos/core/application.hpp>
#include <mythos/input.hpp>
#include <mythos/log.hpp>
#include <mythos/version.hpp>

#include <myl/timer.hpp>

#include <utility>

namespace myth {
    myl::observer_ptr<application> application::s_instance = nullptr;

    MYL_NO_DISCARD constexpr auto application::get() -> application& {
        return *s_instance;
    }
    
    MYL_NO_DISCARD application::application(const application_specification& specs)
        : m_info{ specs.info } {
        MYTHOS_TRACE("Creating application...");
        MYTHOS_INFO("Version: {} ({})", MYTHOS_VERSION_STRING, MYTHOS_VERSION);

        MYTHOS_VERIFY(s_instance.get() == nullptr, "Application has already been created");
        s_instance = this;

        // Initialize all systems

        input::init(specs.input_config);

        // Window may receive bad events if callback is set first
        m_window = window::create(specs.window_config);
        m_event_cb = MYTHOS_BIND_EVENT_FUNC(application::on_event);
        event::set_callback(m_event_cb);

        m_running = true;
        m_suspended = false;

        m_cursor_capturing_window = nullptr; // Window may want to be created while capturing the cursor

        MYTHOS_TRACE("Application created");
    }

    application::~application() {
        MYTHOS_TRACE("Terminating application...");
        m_layer_stack.clear(); // Layers should be destroyed before systems are shutdown

        // Shutdown all systems

        if (!m_cursor_capturing_window)
            release_cursor();
        m_window.reset();

        MYTHOS_TRACE("Application terminated");
    }

    auto application::capture_cursor(window* window, bool hide) -> void {
        if (window == nullptr || window->state() == window_state::minimized)
            return;

        m_cursor_capturing_window = window;
        update_cursor_capture();
        input::set_cursor_visibility(!hide);
    }

    auto application::release_cursor() -> void {
        if (m_cursor_capturing_window) {
            input::release_cursor();
            input::set_cursor_position(m_cursor_capturing_window->position() + (m_cursor_capturing_window->dimensions() / 2));
            input::set_cursor_visibility(true);
            m_cursor_capturing_window = nullptr;
        }
    }

    auto application::push_layer(std::unique_ptr<layer>&& l) -> void {
        m_layer_stack.push(std::move(l));
    }

    auto application::push_overlay(std::unique_ptr<layer>&& o) -> void {
        m_layer_stack.push_overlay(std::move(o));
    }

    auto application::run() -> void {
        myl::high_resolution_stopwatch timer;
        timestep ts = 0;
        timer.reset();

        while(m_running) {
            /// BUG: Keeping track of the time split like this is bad, if the app is paused or the os has
            /// pause it the game loop will not run and a large value will accumulate for ts, this
            /// accumulation should be ignored

            ts = timer.split<std::chrono::seconds, myl::f64>();
            m_stats.timestep = ts;

            if (!m_suspended) {
                for (auto& l : m_layer_stack)
                    l->update(ts);
                for (auto& l : m_layer_stack)
                    l->render();

                input::update();
            }

            m_window->update();
        }
    }

    auto application::quit() noexcept -> void {
        /// TODO: This should send a request to quit event, there may be some operations that can not be interupted.
        m_running = false;
    }

    auto application::update_cursor_capture() -> void {
        if (m_cursor_capturing_window == nullptr)
            return;

        const myl::i32vec2& tl = m_cursor_capturing_window->position();
        input::confine_cursor(tl, tl + m_cursor_capturing_window->dimensions());
        input::set_cursor_position(tl + (m_cursor_capturing_window->dimensions() / 2));
    }

    auto application::on_window_resize(event::window_resize& e) -> bool {
        // Window's values should already be updated
        if (m_cursor_capturing_window && &e.window() == m_cursor_capturing_window.get())
            update_cursor_capture();

        if (&e.window() == m_window.get())
            return true;
        return false;
    }

    auto application::on_window_move(event::window_move& e) -> bool {
        if (m_cursor_capturing_window && &e.window() == m_cursor_capturing_window.get())
            update_cursor_capture();

        if (&e.window() == m_window.get())
            return true;
        return false;
    }
    
    auto application::on_window_close(event::window_close& e) -> bool {
        if (m_cursor_capturing_window && &e.window() == m_cursor_capturing_window.get())
            release_cursor();

        if (&e.window() == m_window.get()) {
            quit();
            return true;
        }

        return false;
    }

    auto application::on_window_focus_gain(event::window_focus_gain& e) -> bool {
        if (&e.window() == m_window.get())
            return true;
        return false;
    }

    auto application::on_window_focus_lost(event::window_focus_lost& e) -> bool {
        if (m_cursor_capturing_window && &e.window() == m_cursor_capturing_window.get())
            release_cursor();

        if (&e.window() == m_window.get())
            return true;
        return false;
    }

    auto application::on_event(event::base& e) -> void {
        event::dispatcher d(e);
        d.dispatch<event::window_resize>(MYTHOS_BIND_EVENT_FUNC(application::on_window_resize));
        d.dispatch<event::window_move>(MYTHOS_BIND_EVENT_FUNC(application::on_window_move));
        d.dispatch<event::window_close>(MYTHOS_BIND_EVENT_FUNC(application::on_window_close));
        d.dispatch<event::window_focus_gain>(MYTHOS_BIND_EVENT_FUNC(application::on_window_focus_gain));
        d.dispatch<event::window_focus_lost>(MYTHOS_BIND_EVENT_FUNC(application::on_window_focus_lost));

        if (!e.handled)
            for (auto& l : m_layer_stack) {
                if (e.handled)
                    break;
                l->on_event(e);
            }
    }
}
