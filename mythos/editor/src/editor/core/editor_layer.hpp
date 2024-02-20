#pragma once
#include <mythos/core/layer.hpp>

namespace editor {
    class editor_layer final : public myth::layer {
    public:
        MYL_NO_DISCARD editor_layer();
        ~editor_layer() override;

        auto on_attach() -> void override;
		auto on_detach() -> void override;
        auto on_event(myth::event::base& e) -> void override;
        auto update(myth::timestep ts) -> void override;
		auto render() -> void override;
    };
}
