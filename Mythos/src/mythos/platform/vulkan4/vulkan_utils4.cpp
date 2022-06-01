#include "vulkan_utils.hpp"

#define MYL_VKRESULT_TO_STRING(result, extended, extended_msg) case result: return std::string(extended ? #result##": "##extended_msg : #result)

namespace myl::vulkan4 {
	std::string VkResult_to_string(VkResult result, bool extended) {
		// From https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkResult.html
		switch (result) {
			MYL_VKRESULT_TO_STRING(VK_SUCCESS, extended, "Command successfully completed");
			MYL_VKRESULT_TO_STRING(VK_NOT_READY, extended, "A fence or query has not yet completed");
			MYL_VKRESULT_TO_STRING(VK_TIMEOUT, extended, "A wait operation has not completed in the specified time");
			MYL_VKRESULT_TO_STRING(VK_EVENT_SET, extended, "An event is signaled");
			MYL_VKRESULT_TO_STRING(VK_EVENT_RESET, extended, "An event is unsignaled");
			MYL_VKRESULT_TO_STRING(VK_INCOMPLETE, extended, "A return array was too small for the result");
			MYL_VKRESULT_TO_STRING(VK_SUBOPTIMAL_KHR, extended, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully");
			MYL_VKRESULT_TO_STRING(VK_THREAD_IDLE_KHR, extended, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call");
			MYL_VKRESULT_TO_STRING(VK_THREAD_DONE_KHR, extended, "A deferred operation is not complete but there is no work remaining to assign to additional threads");
			MYL_VKRESULT_TO_STRING(VK_OPERATION_DEFERRED_KHR, extended, "A deferred operation was requested and at least some of the work was deferred");
			MYL_VKRESULT_TO_STRING(VK_OPERATION_NOT_DEFERRED_KHR, extended, "A deferred operation was requested and no operations were deferred");
			MYL_VKRESULT_TO_STRING(VK_PIPELINE_COMPILE_REQUIRED, extended, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed");

			MYL_VKRESULT_TO_STRING(VK_ERROR_OUT_OF_HOST_MEMORY, extended, "A host memory allocation has failed");
			MYL_VKRESULT_TO_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY, extended, "A device memory allocation has failed");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INITIALIZATION_FAILED, extended, "Initialization of an object could not be completed for implementation-specific reasons");
			MYL_VKRESULT_TO_STRING(VK_ERROR_DEVICE_LOST, extended, "The logical or physical device has been lost. See Lost Device (https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#devsandqueues-lost-device)");
			MYL_VKRESULT_TO_STRING(VK_ERROR_MEMORY_MAP_FAILED, extended, "Mapping of a memory object has failed");
			MYL_VKRESULT_TO_STRING(VK_ERROR_LAYER_NOT_PRESENT, extended, "A requested layer is not present or could not be loaded");
			MYL_VKRESULT_TO_STRING(VK_ERROR_EXTENSION_NOT_PRESENT, extended, "A requested extension is not supported");
			MYL_VKRESULT_TO_STRING(VK_ERROR_FEATURE_NOT_PRESENT, extended, "A requested feature is not supported");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INCOMPATIBLE_DRIVER, extended, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons");
			MYL_VKRESULT_TO_STRING(VK_ERROR_TOO_MANY_OBJECTS, extended, "Too many objects of the type have already been created");
			MYL_VKRESULT_TO_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED, extended, "A requested format is not supported on this device");
			MYL_VKRESULT_TO_STRING(VK_ERROR_FRAGMENTED_POOL, extended, "A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation");
			MYL_VKRESULT_TO_STRING(VK_ERROR_OUT_OF_POOL_MEMORY, extended, "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INVALID_EXTERNAL_HANDLE, extended, "An external handle is not a valid handle of the specified type");
			MYL_VKRESULT_TO_STRING(VK_ERROR_FRAGMENTATION, extended, "A descriptor pool creation has failed due to fragmentation");
			MYL_VKRESULT_TO_STRING(VK_ERROR_SURFACE_LOST_KHR, extended, "A surface is no longer available");
			MYL_VKRESULT_TO_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, extended, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again");
			MYL_VKRESULT_TO_STRING(VK_ERROR_OUT_OF_DATE_KHR, extended, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, extended, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image");
			MYL_VKRESULT_TO_STRING(VK_ERROR_VALIDATION_FAILED_EXT, extended, "");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INVALID_SHADER_NV, extended, "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, extended, "");
			MYL_VKRESULT_TO_STRING(VK_ERROR_NOT_PERMITTED_KHR, extended, "");
			MYL_VKRESULT_TO_STRING(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT, extended, "A buffer creation failed because the requested address is not available");
			// Same as above // MYL_VKRESULT_TO_STRING(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, extended, A buffer creation or memory allocation failed because the requested address is not available.A shader group handle assignment failed because the requested shader group handle information is no longer valid);
			MYL_VKRESULT_TO_STRING(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, extended, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control");
			MYL_VKRESULT_TO_STRING(VK_ERROR_UNKNOWN, extended, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred");
			default: return std::string("Unknown VkResult occured: {}", static_cast<u32>(result));
		}
	}

	bool result_is_success(VkResult result) {
		// From https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkResult.html
		switch (result) {
			case VK_SUCCESS: MYL_FALLTHROUGH;
			case VK_NOT_READY: MYL_FALLTHROUGH;
			case VK_TIMEOUT: MYL_FALLTHROUGH;
			case VK_EVENT_SET: MYL_FALLTHROUGH;
			case VK_EVENT_RESET: MYL_FALLTHROUGH;
			case VK_INCOMPLETE: MYL_FALLTHROUGH;
			case VK_SUBOPTIMAL_KHR: MYL_FALLTHROUGH;
			case VK_THREAD_IDLE_KHR: MYL_FALLTHROUGH;
			case VK_THREAD_DONE_KHR: MYL_FALLTHROUGH;
			case VK_OPERATION_DEFERRED_KHR: MYL_FALLTHROUGH;
			case VK_OPERATION_NOT_DEFERRED_KHR: MYL_FALLTHROUGH;
			case VK_PIPELINE_COMPILE_REQUIRED:
				return true;

			case VK_ERROR_OUT_OF_HOST_MEMORY: MYL_FALLTHROUGH;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY: MYL_FALLTHROUGH;
			case VK_ERROR_INITIALIZATION_FAILED: MYL_FALLTHROUGH;
			case VK_ERROR_DEVICE_LOST: MYL_FALLTHROUGH;
			case VK_ERROR_MEMORY_MAP_FAILED: MYL_FALLTHROUGH;
			case VK_ERROR_LAYER_NOT_PRESENT: MYL_FALLTHROUGH;
			case VK_ERROR_EXTENSION_NOT_PRESENT: MYL_FALLTHROUGH;
			case VK_ERROR_FEATURE_NOT_PRESENT: MYL_FALLTHROUGH;
			case VK_ERROR_INCOMPATIBLE_DRIVER: MYL_FALLTHROUGH;
			case VK_ERROR_TOO_MANY_OBJECTS: MYL_FALLTHROUGH;
			case VK_ERROR_FORMAT_NOT_SUPPORTED: MYL_FALLTHROUGH;
			case VK_ERROR_FRAGMENTED_POOL: MYL_FALLTHROUGH;
			case VK_ERROR_SURFACE_LOST_KHR: MYL_FALLTHROUGH;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: MYL_FALLTHROUGH;
			case VK_ERROR_OUT_OF_DATE_KHR: MYL_FALLTHROUGH;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: MYL_FALLTHROUGH;
			case VK_ERROR_INVALID_SHADER_NV: MYL_FALLTHROUGH;
			case VK_ERROR_OUT_OF_POOL_MEMORY: MYL_FALLTHROUGH;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE: MYL_FALLTHROUGH;
			case VK_ERROR_FRAGMENTATION: MYL_FALLTHROUGH;
			case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: MYL_FALLTHROUGH;
				// Same as above case // VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: MYL_FALLTHROUGH;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: MYL_FALLTHROUGH;
			case VK_ERROR_UNKNOWN: MYL_FALLTHROUGH;
			default:
				return false;
		}
	}
}