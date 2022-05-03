#pragma once
#include "backend.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

/// MYTodo: Make renderer call backend and submit data to backend, this would prevent having to implement a custom renderer per backend

namespace myl::render {
	struct statistics {
		u64 draw_calls = 0;
	};

	class renderer {
		static std::unique_ptr<backend> s_backend;
		static api s_api;
		static statistics s_stats;
	public:
		static void init(const app::info&);
		static void shutdown();

		static backend* backend() { return s_backend.get(); }
		static api api() { return s_api; }
		static const statistics& stats() { return s_stats; }

		static void draw_frame();

		static void on_resize(u32 a_width, u32 a_height);
	};
}