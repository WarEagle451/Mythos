#pragma once
#include "vulkan_context.hpp"
#include "vulkan_swapchain.hpp"

#include <mythos/render/backend.hpp>

/// MYBug: Vulkan crashes if the window starts minimized

namespace myl::vulkane {
	class backend : public render::backend {
		context m_context;
		swapchain m_swapchain;

		std::shared_ptr<render::shader> m_shader;
	public:
		backend();
		~backend();

		backend(const backend&) = delete;
		backend& operator=(const backend&) = delete;

		bool begin() override;
		void end() override;

		std::shared_ptr<render::shader> create_shader(const std::filesystem::path&) override;

		void on_window_resize(const u32vec2&) override;
	};
}