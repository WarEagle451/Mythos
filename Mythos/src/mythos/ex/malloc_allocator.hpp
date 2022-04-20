#pragma once
#include <mythos/defines.hpp>

#include <cstddef>
#include <cstdlib>

namespace myl {
	template<class T>
	struct malloc_allocator {
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = value_type&;
		using const_reference = const reference;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using propagate_on_container_move_assignment = std::true_type;

		MYL_API constexpr malloc_allocator() noexcept {}

		MYL_API constexpr malloc_allocator(const malloc_allocator&) noexcept {}
		
		template<class U>
		constexpr malloc_allocator(const malloc_allocator<U>) noexcept {}

		MYL_API constexpr ~malloc_allocator() {}

		MYL_API MYL_NO_DISCARD constexpr pointer allocate(size_type n) {
			return static_cast<pointer>(malloc(n * sizeof(value_type)));
		}

		MYL_API constexpr void deallocate(pointer ptr, size_type n) {
			free(ptr);
			ptr = NULL;
		}

		/// MYTodo: rebind?
	};
}