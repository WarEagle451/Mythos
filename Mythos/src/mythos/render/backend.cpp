#include "backend.hpp"

#include <mythos/core/assert.hpp>
#include <mythos/platform/vulkan/vulkan_backend.hpp>

namespace myl::render {
	std::unique_ptr<backend> backend::create(api a_api) {
		std::unique_ptr<backend> backend = nullptr;
		switch (a_api) {
			using enum api;
			case none: MYL_CORE_FATAL("No rendering api selected"); break;
			case vulkan: backend = std::make_unique<vulkan::backend>(); break;
			default: MYL_CORE_FATAL("Invalid rendering api"); break;
		}

		MYL_CORE_VERIFY(!backend, "Failed to create renderer backend")
		return backend;
	}
}