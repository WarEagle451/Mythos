#pragma once
#include "backend.hpp"

#include <mythos/defines.hpp>

namespace myl::render {
	struct statistics {
		u64 draw_calls = 0;
	};

	class renderer {
		static std::unique_ptr<backend> s_backend;
		static api s_api;
		static statistics s_stats;
	public:
		static void init();
		static void shutdown();

		static std::unique_ptr<backend>& backend() { return s_backend; }
		static api api() { return s_api; }
		static const statistics& stats() { return s_stats; }

		static void draw_frame();

		/// MYTodo: submit data?

		static void on_resize(u32 a_width, u32 a_height);
	};
}