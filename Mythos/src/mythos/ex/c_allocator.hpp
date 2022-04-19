#pragma once
#include <mythos/defines.hpp>

#include <cstddef>

/// MYTodo: Uses malloc and free

namespace myl {
	template<class T>
	struct c_allocator {
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = value_type&;
		using const_reference = const reference;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using propagate_on_container_move_assignment = std::true_type;

		MYL_API constexpr c_allocator() noexcept;

		MYL_API constexpr c_allocator(const c_allocator&) noexcept;
		
		template<class U>
		constexpr c_allocator(const c_allocator<U>) noexcept;

		MYL_API constexpr ~c_allocator();

		MYL_API MYL_NO_DISCARD constexpr pointer allocate(size_type n) {
			pointer p = alloc(n * sizeof(value_type), );
			if (!p)
				throw core_bad_alloc();
			return p;
		}

		MYL_API constexpr void deallocate(pointer ptr, size_type n) {
			free(ptr);
		}
	};
}