#include "layer_stack.hpp"

namespace myl {
	layer_stack::~layer_stack() {
		for (auto& l : m_layers) { // Safe guard incase shutdown fails somehow
			l->on_detach();
			delete l;
			l = nullptr;
		}
	}

	void layer_stack::clear() {
		m_insert_index = 0;
		while (!m_layers.empty()) {
			m_layers.back()->on_detach();
			delete m_layers.back();
			m_layers.pop_back();
		}
	}

	void layer_stack::push_layer(layer_ptr a_layer) {
		m_layers.emplace(m_layers.begin() + m_insert_index, a_layer);
		m_layers[m_insert_index]->on_attach();
		++m_insert_index;
	}

	void layer_stack::push_overlay(layer_ptr a_overlay) {
		m_layers.emplace_back(a_overlay);
		m_layers.back()->on_attach();
	}

	void layer_stack::pop_layer(layer_ptr a_layer) {
		if (auto it = std::find(m_layers.begin(), m_layers.begin() + m_insert_index, a_layer); it != m_layers.begin() + m_insert_index) {
			a_layer->on_detach();
			m_layers.erase(it);
			--m_insert_index;
		}
	}

	void layer_stack::pop_overlay(layer_ptr a_overlay) {
		if (auto it = std::find(m_layers.begin() + m_insert_index, m_layers.end(), a_overlay); it != m_layers.end()) {
			a_overlay->on_detach();
			m_layers.erase(it);
		}
	}
}