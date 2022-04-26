#include "vulkan_backend.hpp"

#include <mythos/core/log.hpp>

namespace myl::vulkan {
	backend::backend() 
		: m_context() {

		// VkInstance setup
		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.apiVersion = VK_API_VERSION_1_2;
		///MYTodo: app_info.pApplicationName = ;
		///MYTodo: app_info.applicationVersion = VK_MAKE_VERSION();
		app_info.pEngineName = "Mythos Engine";
		app_info.engineVersion = VK_MAKE_VERSION(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH);

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info; // safe because when VkInstance is created &app_info is not needed
		create_info.enabledExtensionCount = 0;
		create_info.ppEnabledExtensionNames = 0;
		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = 0;

		VkResult result = vkCreateInstance(&create_info, nullptr, &m_context.instance());
		result == VK_SUCCESS ?
			MYL_CORE_INFO("vulkan backend initialized") :
			MYL_CORE_ERROR("vkCreateInstance failed with result: {}", static_cast<u32>(result));
	}

	backend::~backend() {

	}

	bool backend::begin() {
		return true;
	}

	void backend::end() {

	}

	void backend::on_window_resize(u32 a_width, u32 a_height) {

	}
}