#pragma once
#include <mythos/defines.hpp>

#include <cmath> ///MYTemp:

namespace myl {
	template<typename T>
	MYL_NO_DISCARD static constexpr T sqrt(T value) {
		return std::sqrt(value); /// MYTodo: Do not use
	}

	/// MYTodo: abs
}