#pragma once
#include <mythos/defines.hpp>

#include <spdlog/spdlog.h>

#include <memory>

namespace myl {
	class loggers {
		static std::shared_ptr<spdlog::logger> s_core;
	public:
		static void init();

		MYL_API MYL_NO_DISCARD static std::shared_ptr<spdlog::logger> get_core() { return s_core; }
	};
}

#ifdef MYL_BUILD_DEBUG
#	define MYL_CORE_TRACE(...) ::myl::loggers::get_core()->trace(__VA_ARGS__)
#	define MYL_CORE_DEBUG(...) ::myl::loggers::get_core()->debug(__VA_ARGS__)
#else
#	define MYL_CORE_TRACE(...)
#	define MYL_CORE_DEBUG(...)
#endif

#define MYL_CORE_INFO(...) ::myl::loggers::get_core()->info(__VA_ARGS__)
#define MYL_CORE_WARN(...) ::myl::loggers::get_core()->warn(__VA_ARGS__)
#define MYL_CORE_ERROR(...) ::myl::loggers::get_core()->error(__VA_ARGS__)
#define MYL_CORE_FATAL(...) ::myl::loggers::get_core()->critical(__VA_ARGS__)