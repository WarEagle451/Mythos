#pragma once
#include <myl/definitions.hpp>

#include <memory>

namespace myth {
    class window {
    public:
        MYL_NO_DISCARD static auto create() -> std::unique_ptr<window>;

        virtual ~window() = default;

        virtual auto update() -> void = 0;
    };
}
