#pragma once
#include <myl/definitions.hpp>

#include <memory>

namespace mythos {
    enum class render_buffer_usage {
        index,
        uniform,
        vertex
    };

    class render_buffer {
    public:
        MYL_NO_DISCARD static auto create(render_buffer_usage usage, myl::usize bytes) -> std::unique_ptr<render_buffer>;

        virtual ~render_buffer() = default;

        virtual auto bind(myl::usize offset = 0, myl::usize index = 0) -> void = 0;
        virtual auto unbind() -> void = 0;

        virtual auto upload(void* data, myl::usize size, myl::usize offset = 0, myl::usize index = 0) -> void = 0;
    };
}
