#pragma once
#include "vulkan_context.hpp"

#include <mythos/render/backend.hpp>

namespace myl::vulkan {
	class backend : public render::backend {
		context m_context;
	public:
		backend();
		~backend();

		bool begin() override;
		void end() override;

		void on_window_resize(u32 a_width, u32 a_height) override;
	};
}