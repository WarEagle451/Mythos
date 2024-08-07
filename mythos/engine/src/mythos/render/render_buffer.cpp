#include <mythos/render/render_buffer.hpp>
#include <mythos/render/renderer.hpp>

namespace myth {
    MYL_NO_DISCARD auto render_buffer::create(render_buffer_usage usage, myl::usize bytes) -> std::unique_ptr<render_buffer> {
        return renderer::backend(usage, bytes)->create_render_buffer();
    }

    auto render_buffer::destroy(render_buffer* buffer) -> void {
        renderer::backend()->destroy_render_buffer(buffer);
    }
}
