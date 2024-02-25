#pragma once
#include <myl/definitions.hpp>
#include <myl/math/vec2.hpp>

#include <memory>

///TODO:
/// - Hide Cursor
/// - creation window type, decorationed, fullscreen, maximized minimized
/// - Is focused
/// paht to icon to load
/// - resize
/// - set position

namespace myth {
    struct window_configuration {
        myl::i32vec2 dimensions = { 720, 600 };
        
        const char* title = "Mythos Application";
    };

    class window {
    public:
        MYL_NO_DISCARD static auto create(const window_configuration& config) -> std::unique_ptr<window>;

        virtual ~window() = default;

        virtual auto update() -> void = 0;
    };
}
