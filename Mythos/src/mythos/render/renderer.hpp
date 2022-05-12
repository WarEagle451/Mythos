#pragma once
#include "backend.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

/// MYTodo: Make renderer call backend and submit data to backend, this would prevent having to implement a custom renderer per backend
/// MYTodo: When resizing the window as long as the window resize has not been let go things can go black, continously update this to allow it to clear the screen everytime

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

		static void draw_frame();

		static void on_window_resize(const u32vec2&);
	};
}