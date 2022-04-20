#pragma once
#include <mythos/defines.hpp>

#include <cstddef>

#include <vector>

namespace myl {
	enum class memory_tag {
		unknown,
		string,
		application,
		texture,
		renderer,
		scene,
		entity,
		array,
		dynamic_array,
		size
	};

	const char* memory_tag_to_string(memory_tag a_tag) {
		switch (a_tag) {
			using enum memory_tag;
			case unknown: return "unknown";
			case string: return "string";
			case application: return "application";
			case texture: return "texture";
			case renderer: return "renderer";
			case scene: return "scene";
			case entity: return "entity";
			case array: return "array";
			case dynamic_array: return "dynamic array";
		}
	}

	class memory_usage {
		static memory_usage s_instance;
	public:
		static const memory_usage& get() { return s_instance; }

		void add(memory_tag a_tag, u64 a_amount) { data[static_cast<u64>(a_tag)] += a_amount; }
		void remove(memory_tag a_tag, u64 a_amount) { data[static_cast<u64>(a_tag)] -= a_amount; }

		u64 bytes(memory_tag a_tag) const { return data[static_cast<u64>(a_tag)]; }
		f64 kib(memory_tag a_tag) const { return static_cast<f64>(data[static_cast<u64>(a_tag)]) / 1024.0; }
		f64 mib(memory_tag a_tag) const { return static_cast<f64>(data[static_cast<u64>(a_tag)]) / (1024.0 * 1024.0); }
		f64 gib(memory_tag a_tag) const { return static_cast<f64>(data[static_cast<u64>(a_tag)]) / (1024.0 * 1024.0 * 1024.0); }

		union {
			u64 data[static_cast<u64>(memory_tag::size)]{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			struct {
				u64 unknown;
				u64 string;
				u64 application;
				u64 texture;
				u64 renderer;
				u64 scene;
				u64 entity;
				u64 array;
				u64 dynamic_array;
			};
		};
	};

	template<class T, memory_tag M>
	struct tag_allocator {
		using value_type = T;
		using memory_type = M;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = value_type&;
		using const_reference = const reference;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using propagate_on_container_move_assignment = std::true_type;

		MYL_API constexpr tag_allocator() noexcept {}

		MYL_API constexpr tag_allocator(const tag_allocator&) noexcept {}

		template<class U>
		constexpr tag_allocator(const tag_allocator<U>) noexcept {}

		MYL_API constexpr ~tag_allocator() {}

		MYL_API MYL_NO_DISCARD constexpr pointer allocate(size_type n) {
			memory_usage::get().add(memory_type, n * sizeof(T));
			return static_cast<pointer>(::operator new(n * sizeof(T)));
		}

		MYL_API constexpr void deallocate(pointer ptr, size_type n) {
			memory_usage::get().remove(memory_type, n * sizeof(T));
			return ::operator delete(static_cast<void*>(ptr));
		}
	};

	std::vector<int, tag_allocator<int, memory_tag::dynamic_array>> i;
}