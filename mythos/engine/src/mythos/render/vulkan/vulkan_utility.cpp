#include <mythos/render/vulkan/vulkan_utility.hpp>

#define MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(result, extended, extended_msg) case result: return extended ? #result " - " extended_msg : #result

namespace myth::vulkan {
    MYL_NO_DISCARD constexpr auto vkresult_string(VkResult code, bool extended_information) -> const char* {
        switch (code) { // From: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkResult.html
            // Success Codes
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_SUCCESS,                        extended_information, "Command successfully completed");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_NOT_READY,                      extended_information, "A fence or query has not yet completed");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_TIMEOUT,                        extended_information, "A wait operation has not completed in the specified time");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_EVENT_SET,                      extended_information, "An event is signaled");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_EVENT_RESET,                    extended_information, "An event is unsignaled");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_INCOMPLETE,                     extended_information, "A return array was too small for the result");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_SUBOPTIMAL_KHR,                 extended_information, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_THREAD_IDLE_KHR,                extended_information, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_THREAD_DONE_KHR,                extended_information, "A deferred operation is not complete but there is no work remaining to assign to additional threads");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_OPERATION_DEFERRED_KHR,         extended_information, "A deferred operation was requested and at least some of the work was deferred");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_OPERATION_NOT_DEFERRED_KHR,     extended_information, "A deferred operation was requested and no operations were deferred");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_PIPELINE_COMPILE_REQUIRED,      extended_information, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed");

            // Error Codes
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_OUT_OF_HOST_MEMORY,                  extended_information, "A host memory allocation has failed");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_OUT_OF_DEVICE_MEMORY,                extended_information, "A device memory allocation has failed");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_INITIALIZATION_FAILED,               extended_information, "Initialization of an object could not be completed for implementation-specific reasons");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_DEVICE_LOST,                         extended_information, "The logical or physical device has been lost. See Lost Device (https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#devsandqueues-lost-devicehttps://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#devsandqueues-lost-device)");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_MEMORY_MAP_FAILED,                   extended_information, "Mapping of a memory object has failed");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_LAYER_NOT_PRESENT,                   extended_information, "A requested layer is not present or could not be loaded");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_EXTENSION_NOT_PRESENT,               extended_information, "A requested extension is not supported");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_FEATURE_NOT_PRESENT,                 extended_information, "A requested feature is not supported");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_INCOMPATIBLE_DRIVER,                 extended_information, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_TOO_MANY_OBJECTS,                    extended_information, "Too many objects of the type have already been created");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_FORMAT_NOT_SUPPORTED,                extended_information, "A requested format is not supported on this device");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_FRAGMENTED_POOL,                     extended_information, "A pool allocation has failed due to fragmentation of the pool's memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_SURFACE_LOST_KHR,                    extended_information, "A surface is no longer available");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,            extended_information, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_OUT_OF_DATE_KHR,                     extended_information, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,            extended_information, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_INVALID_SHADER_NV,                   extended_information, "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_OUT_OF_POOL_MEMORY,                  extended_information, "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_INVALID_EXTERNAL_HANDLE,             extended_information, "An external handle is not a valid handle of the specified type");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_FRAGMENTATION,                       extended_information, "A descriptor pool creation has failed due to fragmentation");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,      extended_information, "A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, extended_information, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exclusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application's control");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_VALIDATION_FAILED_EXT,               extended_information, "A command failed because invalid usage was detected by the implementation or a validation-layer");
            MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL(VK_ERROR_UNKNOWN,                             extended_information, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred");
            default: return "Unhandled VkResult";
        }
    }
}

#undef MYTHOS_VULKAN_VKRESULT_TO_STRING_IMPL
