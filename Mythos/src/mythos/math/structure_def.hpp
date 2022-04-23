#pragma once
#include <cstddef> // std::size_t

namespace myl::math {
	template<std::size_t Size, typename Type> struct vec;

	//@brief Column major to follow mathmatical conventions
	template<std::size_t Rows, std::size_t Columns, typename Type> struct mat;

	///MYTodo: template<typename Type> struct qua;
}