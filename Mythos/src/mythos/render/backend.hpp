#pragma once
#include "common.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

#include <memory>

namespace myl::render {
	class backend {
	public:
		static MYL_NO_DISCARD std::unique_ptr<backend> create(api, const app::info&);

		virtual ~backend() = default;

		virtual bool begin() = 0;
		virtual void end() = 0;

		virtual void on_window_resize(u32 a_width, u32 a_height) = 0;
	};
}