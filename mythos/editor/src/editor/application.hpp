#pragma once
#include <mythos/core/application.hpp>

namespace editor {
    class application : public myth::application {
    public:
        MYL_NO_DISCARD application();
        ~application();
    };
}
