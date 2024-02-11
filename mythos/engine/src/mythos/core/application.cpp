#include <mythos/assert.hpp>
#include <mythos/core/application.hpp>
#include <mythos/log.hpp>
#include <mythos/version.hpp>

#include <utility>

namespace myth {
    myl::observer_ptr<application> application::s_instance = nullptr;

    MYL_NO_DISCARD constexpr auto application::get() -> application& {
        return *s_instance;
    }
    
    MYL_NO_DISCARD application::application(const application_spec& specs)
        : m_info{ specs.info } {
        MYTHOS_TRACE("Creating application...");
        MYTHOS_INFO("Version: {} ({})", MYTHOS_VERSION_STRING, MYTHOS_VERSION);
        MYTHOS_VERIFY(s_instance.get() == nullptr, "Application has already been created");

        s_instance = this;
        m_running = true;
        m_suspended = false;

        // Initialize all systems

        MYTHOS_TRACE("Application created");
    }

    application::~application() {
        MYTHOS_TRACE("Terminating application...");
        m_layer_stack.clear(); // Layers should be destroyed before systems are shutdown

        // Shutdown all systems

        MYTHOS_TRACE("Application terminated");
    }

    auto application::push_layer(std::unique_ptr<layer>&& l) -> void {
        m_layer_stack.push(std::move(l));
    }

    auto application::push_overlay(std::unique_ptr<layer>&& o) -> void {
        m_layer_stack.push_overlay(std::move(o));
    }

    auto application::run() -> void {
        while(m_running) {
            if (!m_suspended) {
                for (auto& l : m_layer_stack)
                    l->update();
                for (auto& l : m_layer_stack)
                    l->render();
            }
            ///TEMP: prevent infinite loop
            m_running = false;
        }
    }

    auto application::quit() noexcept -> void {
        ///TODO: This should send a request to quit event, there may be some operations that can not be interupted.
        m_running = false;
    }
}
