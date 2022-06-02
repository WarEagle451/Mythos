#include "vulkan_fence.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	fence::fence(context& a_context, bool a_signaled)
		: m_context(a_context)
		, m_signaled(a_signaled) {
		VkFenceCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = m_signaled ? VK_FENCE_CREATE_SIGNALED_BIT : static_cast<VkFenceCreateFlags>(0)
		};

		MYL_VK_ASSERT(vkCreateFence, m_context.device(), &info, VK_NULL_HANDLE, &m_handle);
	}

	fence::~fence() {
		if (m_handle)
			vkDestroyFence(m_context.device(), m_handle, VK_NULL_HANDLE);
	}

	bool fence::wait(u64 a_timeout_ns) {
		if (m_signaled)
			return true;
		else switch (vkWaitForFences(m_context.device(), 1, &m_handle, true, a_timeout_ns)) {
			case VK_SUCCESS: m_signaled = true; return true;
			case VK_TIMEOUT:					MYL_CORE_WARN("Vulkan fence - Timed out"); break;
			case VK_ERROR_DEVICE_LOST:			MYL_CORE_ERROR("Vulkan fence - VK_ERROR_DEVICE_LOST"); break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:	MYL_CORE_ERROR("Vulkan fence - VK_ERROR_OUT_OF_HOST_MEMORY"); break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:	MYL_CORE_ERROR("Vulkan fence - VK_ERROR_OUT_OF_DEVICE_MEMORY"); break;
			default:							MYL_CORE_ERROR("Vulkan fence - Unknown error"); break;
		}

		return false;
	}

	void fence::reset() {
		if (m_signaled) {
			MYL_VK_ASSERT(vkResetFences, m_context.device(), 1, &m_handle);
			m_signaled = false;
		}
	}
}