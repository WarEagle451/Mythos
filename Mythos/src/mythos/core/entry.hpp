#pragma once
#include "app.hpp"

namespace myl { //@brief To be defined in client
	MYL_API MYL_NO_DISCARD extern std::unique_ptr<app> create();
}

#include "assert.hpp"
#include "except.hpp"

int main() {
	try {
		std::unique_ptr<myl::app> app = myl::create();
		MYL_CORE_ASSERT(app, "Failed to create application");
		app->run();
	}
	catch (myl::core_exception& e) { MYL_CORE_FATAL(e.message()); MYL_DEBUGBREAK(); return EXIT_FAILURE; }
	catch (std::exception& e) { MYL_CORE_FATAL(e.what()); MYL_DEBUGBREAK(); return EXIT_FAILURE; }
	catch (...) { MYL_CORE_FATAL("An exception occured"); MYL_DEBUGBREAK(); return EXIT_FAILURE; }
	return EXIT_SUCCESS;
}