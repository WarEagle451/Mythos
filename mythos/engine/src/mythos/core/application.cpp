#include <mythos/assert.hpp>
#include <mythos/core/application.hpp>
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

        m_window = window::create();

        m_running = true;
        m_suspended = false;

        // Initialize all systems

        MYTHOS_TRACE("Application created");
    }

    application::~application() {
        MYTHOS_TRACE("Terminating application...");
        m_layer_stack.clear(); // Layers should be destroyed before systems are shutdown

        // Shutdown all systems

        m_window.reset();

        MYTHOS_TRACE("Application terminated");
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
            /// pause it the game loop will not run and a large value will acculate for ts

            ts = timer.split<std::chrono::seconds, myl::f64>();
            m_stats.timestep = ts;

            if (!m_suspended) {
                for (auto& l : m_layer_stack)
                    l->update(ts);
                for (auto& l : m_layer_stack)
                    l->render();
            }

            m_window->update();

            /// TEMP: prevent infinite loop
            //m_running = false;
        }
    }

    auto application::quit() noexcept -> void {
        /// TODO: This should send a request to quit event, there may be some operations that can not be interupted.
        m_running = false;
    }
}
