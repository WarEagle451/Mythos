#pragma once
#include <mythos/core/layer.hpp>

#include <myl/definitions.hpp>

#include <memory>
#include <vector>

namespace mye {
    class layer_stack {
        std::vector<std::unique_ptr<layer>> m_layers;
        std::vector<std::unique_ptr<layer>>::size_type m_layer_insert_index = 0;
    public:
        MYL_NO_DISCARD constexpr layer_stack() noexcept = default;
        ~layer_stack();

        auto clear() -> void;

        auto push(std::unique_ptr<layer>&& l) -> void;
        auto push_overlay(std::unique_ptr<layer>&& o) -> void;
        auto pop(layer* l) -> void;

        MYL_NO_DISCARD constexpr auto begin() { return m_layers.begin(); }
        MYL_NO_DISCARD constexpr auto begin() const { return m_layers.begin(); }
        MYL_NO_DISCARD constexpr auto end() { return m_layers.end(); }
        MYL_NO_DISCARD constexpr auto end() const { return m_layers.begin(); }
    };
}
