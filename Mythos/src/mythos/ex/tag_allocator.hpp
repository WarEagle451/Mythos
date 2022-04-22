#pragma once
#include <mythos/defines.hpp>

#include <cstddef>

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

	MYL_API MYL_NO_DISCARD const char* memory_tag_to_string(memory_tag a_tag) {
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
		MYL_API MYL_NO_DISCARD static const memory_usage& get() { return s_instance; }

		MYL_API void add(memory_tag a_tag, u64 a_amount) { data[static_cast<u64>(a_tag)] += a_amount; }
		MYL_API void remove(memory_tag a_tag, u64 a_amount) { data[static_cast<u64>(a_tag)] -= a_amount; }

		MYL_API MYL_NO_DISCARD u64 bytes(memory_tag a_tag) const { return data[static_cast<u64>(a_tag)]; }
		MYL_API MYL_NO_DISCARD f64 kib(memory_tag a_tag) const { return static_cast<f64>(data[static_cast<u64>(a_tag)]) / 1024.0; }
		MYL_API MYL_NO_DISCARD f64 mib(memory_tag a_tag) const { return static_cast<f64>(data[static_cast<u64>(a_tag)]) / (1024.0 * 1024.0); }
		MYL_API MYL_NO_DISCARD f64 gib(memory_tag a_tag) const { return static_cast<f64>(data[static_cast<u64>(a_tag)]) / (1024.0 * 1024.0 * 1024.0); }

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

	template<class T, memory_tag Tag>
	struct tag_allocator {
		using value_type = T;
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
		constexpr tag_allocator(const tag_allocator<U, Tag>) noexcept {}

		MYL_API constexpr ~tag_allocator() {}

		MYL_API MYL_NO_DISCARD constexpr pointer allocate(size_type n) {
			memory_usage::get().add(Tag, n * sizeof(T));
			return static_cast<pointer>(::operator new(n * sizeof(T)));
		}

		MYL_API constexpr void deallocate(pointer ptr, size_type n) {
			memory_usage::get().remove(Tag, n * sizeof(T));
			return ::operator delete(static_cast<void*>(ptr));
		}
	};
}