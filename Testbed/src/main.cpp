#include <mythos/core/log.hpp>

#include <mythos/core/assert.hpp>

int main() {
	myl::core::logger_instances::init();
	
	MYL_CORE_TRACE("This is a {}", "trace");
	MYL_CORE_DEBUG("This is a {}", "debug");
	MYL_CORE_INFO("This is a {}", "info");
	MYL_CORE_WARN("This is a {}", "warn");
	MYL_CORE_ERROR("This is a {}", "error");
	MYL_CORE_FATAL("This is a {}", "fatal");

	MYL_CORE_ASSERT(false);
}