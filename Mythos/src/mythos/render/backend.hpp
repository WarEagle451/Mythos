#pragma once
#include "common.hpp"
#include "shader.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

#include <memory>

namespace myl::render {
	class backend {
	public:
		static MYL_NO_DISCARD std::unique_ptr<backend> create(api);

		virtual ~backend() = default;

		virtual bool begin() = 0;
		virtual void end() = 0;

		virtual std::shared_ptr<shader> create_shader(const std::filesystem::path&) = 0;

		virtual void on_window_resize(const u32vec2&) = 0;
	};
}