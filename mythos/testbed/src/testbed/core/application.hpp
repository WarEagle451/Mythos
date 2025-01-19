#pragma once
#include <mythos/core/application.hpp>

namespace testbed {
    class application : public mye::application {
    public:
        MYL_NO_DISCARD application(const mye::application_specification& specs);
        ~application();
    };
}
