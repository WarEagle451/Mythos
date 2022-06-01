#pragma once
#include "vulkan_context.hpp"

#include <mythos/render/backend.hpp>

void vulkan_renderer_backend_shutdown(myl::vulkan4::context&);

namespace myl::vulkan4 {
	class backend : public render::backend {
	public: /// MYTemp: until above is fixed
		context m_context;
	public:
		backend();
		~backend();

		backend(const backend&) = delete;
		backend& operator=(const backend&) = delete;

		bool begin() override;
		void end() override;

		void on_window_resize(const u32vec2&) override;

		std::shared_ptr<render::shader> create_shader(const std::filesystem::path&) override;
	};
}