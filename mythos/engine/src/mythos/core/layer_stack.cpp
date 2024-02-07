#include <mythos/core/layer_stack.hpp>
#include <mythos/log.hpp>

#include <algorithm>

namespace myth {
    layer_stack::~layer_stack() {
        clear();
    }

    auto layer_stack::clear() -> void {
        while (!m_layers.empty()) {
            m_layers.back()->on_detach();
            m_layers.pop_back();
        }
        m_layer_insert_index = 0;
    }

    auto layer_stack::push(std::unique_ptr<layer>&& l) -> void {
        m_layers.emplace(m_layers.begin() + m_layer_insert_index, std::move(l));
        m_layers[m_layer_insert_index]->on_attach();
        ++m_layer_insert_index;
    }

    auto layer_stack::push_overlay(std::unique_ptr<layer>&& o) -> void {
        m_layers.emplace_back(std::move(o));
        m_layers.back()->on_attach();
    }

    auto layer_stack::pop(layer* l) -> void {
        //auto it = std::find(m_layers.begin(), m_layers.end(), l);
        // Must use std::find_if because std::find doesn't work with std::unique_ptr
        auto it = std::find_if(m_layers.begin(), m_layers.end(), [l](std::unique_ptr<layer>& p) -> bool { return p.get() == l; });
        if (it == m_layers.end()) {
            MYTHOS_ERROR("Layer stack doesn't contain layer/overlay '{}'", l->name());
            return;
        }

        if (it < m_layers.begin() + m_layer_insert_index) // it is a layer. Popping overlays doesn't decrement m_layer_insert_index
            --m_layer_insert_index;
        l->on_detach();
		m_layers.erase(it);
    }
}
