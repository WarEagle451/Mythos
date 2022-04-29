#include "backend.hpp"

#include <mythos/core/assert.hpp>
#include <mythos/core/log.hpp>
#include <mythos/core/except.hpp>

#include <mythos/platform/vulkan/vulkan_backend.hpp>

namespace myl::render {
	std::unique_ptr<backend> backend::create(api a_api, const  app::info& a_info) {
		std::unique_ptr<backend> backend = nullptr;
		switch (a_api) {
			using enum api;
			case none: MYL_CORE_ASSERT("no rendering api selected"); break;
			case vulkan: backend = std::make_unique<vulkan::backend>(a_info); break;
			case opengl: MYL_CORE_ASSERT("opengl is currently not supported"); break;
			case metal: MYL_CORE_ASSERT("metal is currently not supported"); break;
			default: MYL_CORE_ASSERT("invalid rendering api"); break;
		}

		if (!backend)
			throw core_runtime_error("failed to create renderer backend");
		return backend;
	}
}