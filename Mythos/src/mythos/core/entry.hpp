#pragma once
#include "app.hpp"

namespace myl {
	MYL_API extern std::unique_ptr<app> create(); // to be defined in client
}

#include "entry.hpp"
#include "except.hpp"
#include "log.hpp"
#include "assert.hpp"

int main() {
	try {
		std::unique_ptr<myl::app> app = myl::create();
		MYL_CORE_ASSERT(app, "failed to create application");
		app->run();
	}
	catch (myl::core_exception& e) { MYL_CORE_FATAL(e.message()); return EXIT_FAILURE; }
	catch (std::exception& e) { MYL_CORE_FATAL(e.what()); return EXIT_FAILURE; }
	catch (...) { MYL_CORE_FATAL("an exception occured"); return EXIT_FAILURE; }
	return EXIT_SUCCESS;
}