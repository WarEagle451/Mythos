#include "vulkan_fence.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	fence::fence(context& a_context, bool a_create_signaled)
		: m_context(a_context)
		, m_signaled(a_create_signaled) {
		VkFenceCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};

		MYL_VK_ASSERT(vkCreateFence, m_context.m_device, &info, VK_NULL_HANDLE, &m_handle);
	}

	fence::~fence() {
		if (m_handle)
			vkDestroyFence(m_context.m_device, m_handle, VK_NULL_HANDLE);
	}

	bool fence::wait(u64 a_timeout_ns) {
		if (!m_signaled) {
			VkResult result = vkWaitForFences(m_context.m_device, 1, &m_handle, true, a_timeout_ns);
			switch (result) {
				case VK_SUCCESS:
					m_signaled = true;
					return true;
				case VK_TIMEOUT:
					MYL_CORE_WARN("vk_fence_wait - Timed out");
					break;
				case VK_ERROR_DEVICE_LOST:
					MYL_CORE_ERROR("vk_fence_wait - VK_ERROR_DEVICE_LOST.");
					break;
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					MYL_CORE_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY.");
					break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					MYL_CORE_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
					break;
				default:
					MYL_CORE_ERROR("vk_fence_wait - An unknown error has occurred.");
					break;
			}
		}
		else return true; // If already signaled, do not wait.
		return false;
	}

	void fence::reset() {
		if (m_signaled) {
			MYL_VK_ASSERT(vkResetFences, m_context.m_device, 1, &m_handle);
			m_signaled = false;
		}
	}
}