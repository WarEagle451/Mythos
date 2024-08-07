#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>
#include <mythos/render/vulkan/vulkan_platform.hpp>
#include <mythos/render/vulkan/vulkan_render_buffer.hpp>
#include <mythos/render/vulkan/vulkan_shader.hpp>>
#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/version.hpp>

/// MYTODO: Continue Vulkan tutorial from;
/// - https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer

namespace myth::vulkan {
#ifdef VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    static VKAPI_ATTR auto debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) -> VkBool32 {
        /// MYTODO: Extend Vulkan debug callback with the following:
        /// - type
        ///    - VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT - Ignore
        ///    - VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT - Violates spec or possible mistake
        ///    - VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT - Possible non-optimal use of Vulkan
        /// - callback_data
        ///    - pObjects - Array of Vulkan objects related to the message
        ///    - objectCount - Number of objects
        
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: MYTHOS_TRACE("Validation Layer - {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    MYTHOS_INFO("Validation Layer - {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MYTHOS_WARN("Validation Layer - {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   MYTHOS_ERROR("Validation Layer - {}", callback_data->pMessage); break;
            default:                                              MYTHOS_ERROR("Validation Layer - {}", callback_data->pMessage); break;
        }
        return VK_FALSE;
    }
#endif

    static auto get_required_validation_layers(std::vector<const char*>* required) -> void {
#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        required->emplace_back("VK_LAYER_KHRONOS_validation");

        uint32_t count = 0;
        vkEnumerateInstanceLayerProperties(&count, VK_NULL_HANDLE);
        std::vector<VkLayerProperties> available(count);
        vkEnumerateInstanceLayerProperties(&count, available.data());

#ifdef MYL_DEBUG
        if (!required->empty()) {
            MYTHOS_DEBUG("Required Vulkan validation layers:");
            for (const auto& str : *required)
                MYTHOS_DEBUG("\t- {}", str);
        }

        if (!available.empty()) {
            MYTHOS_DEBUG("Available Vulkan validation layers:");
            for (const auto& l : available)
                MYTHOS_DEBUG("\t- {}", l.layerName);
        }
#endif

        for (const auto& r : *required) {
            bool found = false;
            for (const auto& a : available)
                if (strcmp(r, a.layerName) == 0) {
                    found = true;
                    break;
                }

            if (!found)
                MYTHOS_FATAL("Required validation layer '{}' not found", r);
        }
#endif
    }

    static auto get_required_extensions(std::vector<const char*>* required) -> void {
        required->emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
        get_platform_required_extensions(required);

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        required->emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
#ifdef MYL_DEBUG
        if (!required->empty()) {
            MYTHOS_DEBUG("Required Vulkan extensions:");
            for (const auto& str : *required)
                MYTHOS_DEBUG("\t- {}", str);
        }

        uint32_t available_count = 0;
        vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &available_count, VK_NULL_HANDLE);
        std::vector<VkExtensionProperties> available(available_count);
        vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &available_count, available.data());

        if (!available.empty()) {
            MYTHOS_DEBUG("Available Vulkan extensions:");
            for (const auto& ex : available)
                MYTHOS_DEBUG("\t- {}", ex.extensionName);
        }
#endif
    }

    MYL_NO_DISCARD backend::backend(const renderer_configuration& config)
        : m_current_frame_index{ 0 }
        , m_framebuffer_resized{ false }
        , m_vsync{ config.vsync } {
        myth::window* window = application::get().main_window();

        create_instance();
        create_surface(&m_surface, m_instance, window, VK_NULL_HANDLE);

        device::create_info device_create_info{
            .instance                     = m_instance,
            .surface                      = m_surface,
            .physical_device_requirements = { /// MYTEMP: physical_device_requirements should be user dependent
                .discrete_gpu   = true,
                .queue_compute  = false,
                .queue_graphics = true,
                .queue_present  = true,
                .queue_transfer = false,
                .extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }
            }
        };

        device::create(&m_device, device_create_info, VK_NULL_HANDLE);

        m_vsync = config.vsync; /// MYTODO: Improve this
        m_cached_vsync = m_vsync;
        const auto& window_extent = window->framebuffer_size();
        swapchain::create_info swapchain_create_info{
            .surface = m_surface,
            .extent  = VkExtent2D{
                .width  = static_cast<uint32_t>(window_extent.w),
                .height = static_cast<uint32_t>(window_extent.h)
            },
            .vsync   = m_vsync
        };
        
        swapchain::create(&m_swapchain, m_device, swapchain_create_info, VK_NULL_HANDLE);
        m_cached_framebuffer_extent = m_swapchain.image_extent();
        
        render_pass::create_info render_pass_create_info{
            .color_format = m_swapchain.image_format().format,
            .clear_color  = VkClearColorValue{
                .float32{
                    static_cast<float>(config.clear_color.r),
                    static_cast<float>(config.clear_color.g),
                    static_cast<float>(config.clear_color.b),
                    1.f
                }
            }
        };
        
        render_pass::create(&m_main_render_pass, m_device, render_pass_create_info, VK_NULL_HANDLE);

        command_pool::create_info command_pool_create_info{
            .queue_family_index = m_device.queue_family_indices().graphics
        };

        command_pool::create(&m_command_pool, m_device, command_pool_create_info, VK_NULL_HANDLE);

        m_swapchain.recreate_framebuffers(m_device, m_main_render_pass.handle(), VK_NULL_HANDLE);
        m_command_buffers.resize(m_swapchain.max_frames_in_flight());
        m_command_pool.allocate_command_buffers(m_device, m_command_buffers.begin(), m_command_buffers.end());
    }

    backend::~backend() {
        vkDeviceWaitIdle(m_device.logical());

        m_command_pool.deallocate_command_buffers(m_device, m_command_buffers.begin(), m_command_buffers.end());
        m_command_buffers.clear();

        m_swapchain.destroy_framebuffers(m_device, VK_NULL_HANDLE);

        command_pool::destroy(&m_command_pool, m_device, VK_NULL_HANDLE);
        render_pass::destroy(&m_main_render_pass, m_device, VK_NULL_HANDLE);
        swapchain::destroy(&m_swapchain, m_device, VK_NULL_HANDLE);
        device::destroy(&m_device, VK_NULL_HANDLE);
        vkDestroySurfaceKHR(m_instance, m_surface, VK_NULL_HANDLE);
        destroy_instance();
    }

    MYL_NO_DISCARD auto backend::create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, const shader_layout& layout, shader_primitive primitive) -> std::unique_ptr<myth::shader> {
        vulkan::shader::create_info shader_create_info{
            .swapchain_extent = m_swapchain.image_extent(),
            .render_pass      = m_main_render_pass.handle(),
            .binaries         = shader_binaries,
            .layout           = layout,
            .primitive        = primitive
        };

        std::unique_ptr<vulkan::shader> shader = std::make_unique<vulkan::shader>();
        vulkan::shader::create(shader.get(), m_device, shader_create_info, VK_NULL_HANDLE);
        return shader;
    }

    auto backend::destroy_shader(myth::shader* shader) -> void {
        vulkan::shader* vks = static_cast<vulkan::shader*>(shader);
        vulkan::shader::destroy(vks, m_device, VK_NULL_HANDLE);
    }

    MYL_NO_DISCARD auto backend::create_render_buffer(render_buffer_usage usage, myl::usize bytes) -> std::unique_ptr<myth::render_buffer> {
        vulkan::render_buffer::create_info render_buffer_create_info{
            .usage                = render_buffer_usage_to_VkBufferUsageFlags(usage),
            .bytes                = static_cast<VkDeviceSize>(bytes),
            ///.properties           = ,
            .sharing_mode         = VK_SHARING_MODE_EXCLUSIVE
        };

        std::unique_ptr<vulkan::render_buffer> render_buffer = std::make_unique<vulkan::render_buffer>();
        vulkan::render_buffer::create(render_buffer.get(), m_device, render_buffer_create_info, VK_NULL_HANDLE);
        return render_buffer;
    }

    auto backend::destroy_render_buffer(myth::render_buffer* buffer) -> void {
        vulkan::render_buffer* vkrb = static_cast<vulkan::render_buffer*>(buffer);
        vulkan::render_buffer::destroy(vkrb, m_device, VK_NULL_HANDLE);
    }

    auto backend::set_clear_color(const myl::f32vec3& color) -> void {
        m_main_render_pass.set_clear_color(VkClearColorValue{
            .float32{
                static_cast<float>(color.r),
                static_cast<float>(color.g),
                static_cast<float>(color.b),
                1.f
            }
        });
    }

    auto backend::set_vsync(bool value) -> void {
        m_cached_vsync = value;
    }

    auto backend::begin_frame() -> bool {
        // Conditions for a bad frame

        // Zero sized framebuffers are illegal in Vulkan
        if (m_cached_framebuffer_extent.width == 0 ||
            m_cached_framebuffer_extent.height == 0 ||
            m_cached_vsync != m_vsync) {
            swapchain::create_info swapchain_create_info{
                .surface = m_surface,
                .extent  = m_cached_framebuffer_extent,
                .vsync   = m_cached_vsync
            };

            m_swapchain.recreate(&m_swapchain, m_device, m_main_render_pass.handle(), swapchain_create_info, VK_NULL_HANDLE);
            m_vsync = m_cached_vsync;
            return false;
        }


        /// MYTODO: if skip by minimization is needed then is should be done here

        // Wait for previous frame to stop rendering

        VkFence& current_frame_fence = m_swapchain.in_flight_fences()[m_current_frame_index];
        vkWaitForFences(m_device.logical(), 1, &current_frame_fence, VK_TRUE, UINT64_MAX);

        // Acquire next image

        VkResult acquire_next_image_result = m_swapchain.acquire_next_image(m_device, m_swapchain.image_available_semaphores()[m_current_frame_index]);
        switch (acquire_next_image_result) {
            case VK_SUCCESS:
                break;
            case VK_SUBOPTIMAL_KHR:
                break; /// MYTODO: Should the swapchain recreate if it is suboptimal?
            case VK_ERROR_OUT_OF_DATE_KHR: {
                swapchain::create_info swapchain_create_info{
                    .surface = m_surface, 
                    .extent  = m_cached_framebuffer_extent
                };

                swapchain::recreate(&m_swapchain, m_device, m_main_render_pass.handle(), swapchain_create_info, VK_NULL_HANDLE);
                m_framebuffer_resized = false;
                return false;
            }
            default:
                MYTHOS_FATAL("Failed to acquire swapchain image: {}", vkresult_string(acquire_next_image_result, true));
                return false;
        }

        vkResetFences(m_device.logical(), 1, &current_frame_fence); // Only reset fence if work is being submitted

        VkRect2D scissor{
            .offset = { .x = 0, .y = 0 },
            .extent = m_swapchain.image_extent()
        };
        
        VkViewport viewport{
            .x        = 0.f,
            .y        = 0.f,
            .width    = static_cast<float>(scissor.extent.width),
            .height   = static_cast<float>(scissor.extent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f
        };
        
        command_buffer& current_command_buffer = m_command_buffers[m_current_frame_index];
        current_command_buffer.reset();
        current_command_buffer.begin();
        m_main_render_pass.begin(current_command_buffer.handle(), m_swapchain.framebuffers()[m_swapchain.current_image_index()], scissor);
        
        // Record commands

        vkCmdSetViewport(current_command_buffer.handle(), 0, 1, &viewport);
        vkCmdSetScissor(current_command_buffer.handle(), 0, 1, &scissor);

        return true;
    }

    auto backend::end_frame() -> void {
        command_buffer& current_command_buffer = m_command_buffers[m_current_frame_index];
        
        m_main_render_pass.end(current_command_buffer.handle());
        current_command_buffer.end();

        VkSemaphore wait_semaphores[] = {
            m_swapchain.image_available_semaphores()[m_current_frame_index]
        };

        VkPipelineStageFlags wait_stages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSemaphore signal_semaphores[] = {
            m_swapchain.render_finished_semaphores()[m_current_frame_index]
        };

        // Submit commands

        VkSubmitInfo submit_info{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            //.pNext                =,
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = wait_semaphores,
            .pWaitDstStageMask    = wait_stages,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &current_command_buffer.handle(),
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = signal_semaphores
        };

        MYTHOS_VULKAN_VERIFY(vkQueueSubmit, m_device.graphics_queue(), 1, &submit_info, m_swapchain.in_flight_fences()[m_current_frame_index]);

        // Presentation

        const VkResult present_result = m_swapchain.present(m_device.present_queue(), signal_semaphores);
        if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || m_framebuffer_resized) {
            swapchain::create_info swapchain_create_info{
                .surface = m_surface,
                .extent = m_cached_framebuffer_extent,
                .vsync = m_cached_vsync
            };

            swapchain::recreate(&m_swapchain, m_device, m_main_render_pass.handle(), swapchain_create_info, VK_NULL_HANDLE);
            m_framebuffer_resized = false;
        }
        else if (present_result != VK_SUCCESS)
            MYTHOS_FATAL("Failed to present swapchain image: {}", vkresult_string(present_result, true));

        m_current_frame_index = (m_current_frame_index + 1) % m_swapchain.max_frames_in_flight();
    }

    auto backend::draw(draw_data& draw_data) -> void {
        // Get current command buffer
        command_buffer& current_command_buffer = m_command_buffers[m_current_frame_index];
        vulkan::render_buffer& vkrb = static_cast<vulkan::render_buffer&>(draw_data.vertex_buffer);
        VkDeviceSize offsets[]{ 0 };
        
        vulkan::shader& vk_shader = static_cast<vulkan::shader&>(draw_data.shader);
        vk_shader.bind(current_command_buffer.handle());

        vkCmdBindVertexBuffers(current_command_buffer.handle(), 0, 1, &vkrb.handle(), offsets);

        vkCmdDraw(current_command_buffer.handle(), static_cast<uint32_t>(draw_data.vertex_count), 1, 0, 0);
    }

    auto backend::prepare_shutdown() -> void {
        vkDeviceWaitIdle(m_device.logical());
    }

    auto backend::on_window_resize(const myl::i32vec2& dimensions) -> void {
        /// MYTODO: Vulkan is still rendering the window when minimized, this should not occur
        if (application::get().main_window()->state() == window_state::minimized) // Some platforms may send a window_resize event upon window minimization, don't warn on that
            return;

        // Begin frame handles if width or height == 0

        m_framebuffer_resized = true;
        m_cached_framebuffer_extent = VkExtent2D{
            .width  = static_cast<uint32_t>(dimensions.w),
            .height = static_cast<uint32_t>(dimensions.h)
        };
    }

    auto backend::create_instance() -> void {
        const application_information& mythos_app_info = application::get().info();
        VkApplicationInfo application_info{
            .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            //.pNext = ,
            .pApplicationName   = mythos_app_info.name,
            .applicationVersion = VK_MAKE_API_VERSION(0, mythos_app_info.version_major, mythos_app_info.version_minor, mythos_app_info.version_patch),
            .pEngineName        = MYTHOS_ENGINE_NAME,
            .engineVersion      = VK_MAKE_API_VERSION(0, MYTHOS_VERSION_MAJOR, MYTHOS_VERSION_MINOR, MYTHOS_VERSION_PATCH),
            .apiVersion         = VK_API_VERSION_1_3
        };

        std::vector<const char*> validation_layers;
        get_required_validation_layers(&validation_layers);

        std::vector<const char*> extensions;
        get_required_extensions(&extensions);

        VkInstanceCreateInfo instance_create_info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .pApplicationInfo        = &application_info,
            .enabledLayerCount       = static_cast<uint32_t>(validation_layers.size()),
            .ppEnabledLayerNames     = validation_layers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            //.pNext = ,
            //.flags = ,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData       = VK_NULL_HANDLE // Passing a pointer here would allow that pointer to be used in the debug callback
        };
        instance_create_info.pNext = &debug_messenger_create_info;
#endif
        MYTHOS_VULKAN_VERIFY(vkCreateInstance, &instance_create_info, VK_NULL_HANDLE, &m_instance);

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
        if (vkCreateDebugUtilsMessengerEXT == nullptr)
            MYTHOS_FATAL("Failed to load 'vkCreateDebugUtilsMessengerEXT'");
        MYTHOS_VULKAN_VERIFY(vkCreateDebugUtilsMessengerEXT, m_instance, &debug_messenger_create_info, VK_NULL_HANDLE, &m_debug_messenger);
#endif
    }

    auto backend::destroy_instance() -> void {
#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (vkDestroyDebugUtilsMessengerEXT == nullptr)
            MYTHOS_FATAL("Failed to load 'vkDestroyDebugUtilsMessengerEXT'");
        else
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, VK_NULL_HANDLE);
#endif

        vkDestroyInstance(m_instance, VK_NULL_HANDLE);
    }
}
