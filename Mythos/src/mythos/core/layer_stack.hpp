#pragma once
#include "layer.hpp"

#include <mythos/defines.hpp>

#include <vector>
#include <memory>

namespace myl {
	class layer_stack {
	public:
		using layer_ptr = layer*; /// MYTodo: get these to be unique ptrs
		using stack = std::vector<layer_ptr>;
	private:
		stack m_layers;
		u32 m_insert_index = 0;
	public:
		layer_stack() = default;
		~layer_stack();

		void clear();

		void push_layer(layer_ptr a_layer);
		void push_overlay(layer_ptr a_overlay);
		void pop_layer(layer_ptr a_layer);
		void pop_overlay(layer_ptr a_overlay);

		stack::iterator begin() { return m_layers.begin(); }
		stack::iterator end() { return m_layers.end(); }
		stack::reverse_iterator rbegin() { return m_layers.rbegin(); }
		stack::reverse_iterator rend() { return m_layers.rend(); }
		
		stack::const_iterator begin() const { return m_layers.begin(); }
		stack::const_iterator end() const { return m_layers.end(); }
		stack::const_reverse_iterator rbegin() const { return m_layers.rbegin(); }
		stack::const_reverse_iterator rend() const { return m_layers.rend(); }
	};
}