#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context; // fwd declaration

	class fence {
		context& m_context;

		VkFence m_handle;
		bool m_signaled;
	public:
		fence(context&, bool a_signaled);
		~fence();

		fence(const fence&) = delete;
		fence& operator=(const fence&) = delete;

		VkFence& handle() { return m_handle; }

		bool wait(u64 a_timeout_ns = ~0);
		void reset();
	};
}