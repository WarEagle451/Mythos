#pragma once
#include <mythos/core/application.hpp>

namespace testbed {
    class application : public myth::application {
    public:
        MYL_NO_DISCARD application(const myth::application_specification& specs);
        ~application();
    };
}
