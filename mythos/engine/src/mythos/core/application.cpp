#include <mythos/core/application.hpp>

namespace myth {
    extern auto create_application() -> std::unique_ptr<application> {}

    myl::observer_ptr<application> application::s_instance = nullptr;

    auto application::set(const myl::observer_ptr<application>& ptr) -> void {
        s_instance = ptr;
    }

    MYL_NO_DISCARD auto application::get() -> application& {
        return *s_instance;
    }
    
    MYL_NO_DISCARD application::application() {

    }

    application::~application() {

    }

    auto application::run() -> void {

    }
}
