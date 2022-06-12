#pragma once
#include "backend.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

namespace myl::render {
	struct statistics {
		u64 draw_calls = 0;
	};

	class renderer {
		static std::unique_ptr<backend> s_backend;
		static api s_api;
		static statistics s_stats;
	public:
		//@brief Will auto detect api if none is selected
		static void init(api = api::none);
		static void shutdown();

		static MYL_NO_DISCARD backend* backend() { return s_backend.get(); }
		static MYL_NO_DISCARD api api() { return s_api; }
		static MYL_NO_DISCARD const statistics& stats() { return s_stats; }

		static bool begin(); /// MYTodo: Needs to take camera
		static void end();

		static void on_window_resize(const u32vec2&);
	};
}