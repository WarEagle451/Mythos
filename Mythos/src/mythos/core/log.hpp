#pragma once
#include <mythos/defines.hpp>

#include <spdlog/spdlog.h>

#include <memory>

namespace myl::core { /// MYTodo: Replace with custom logger
	class logger_instances {
		static std::shared_ptr<spdlog::logger> s_core;
	public:
		MYL_API static void init();

		MYL_API MYL_NO_DISCARD static std::shared_ptr<spdlog::logger> get_core() { return s_core; }
	};
}

#ifdef MYL_DEBUG_BUILD
#	define MYL_CORE_TRACE(...) ::myl::core::logger_instances::get_core()->trace(__VA_ARGS__)
#	define MYL_CORE_DEBUG(...) ::myl::core::logger_instances::get_core()->debug(__VA_ARGS__)
#else
#	define MYL_CORE_TRACE(...)
#	define MYL_CORE_DEBUG(...)
#endif

#define MYL_CORE_INFO(...) ::myl::core::logger_instances::get_core()->info(__VA_ARGS__)
#define MYL_CORE_WARN(...) ::myl::core::logger_instances::get_core()->warn(__VA_ARGS__)
#define MYL_CORE_ERROR(...) ::myl::core::logger_instances::get_core()->error(__VA_ARGS__)
#define MYL_CORE_FATAL(...) ::myl::core::logger_instances::get_core()->critical(__VA_ARGS__)

#include <string>
namespace myl::log {
	template<typename... Args> void trace(std::string_view a_format, Args&&... a_args) { core::logger_instances::get_core()->trace(a_format, std::forward<Args>(a_args)...); }
	template<typename... Args> void debug(std::string_view a_format, Args&&... a_args) { core::logger_instances::get_core()->debug(a_format, std::forward<Args>(a_args)...); }
	template<typename... Args> void info(std::string_view a_format, Args&&... a_args) { core::logger_instances::get_core()->info(a_format, std::forward<Args>(a_args)...); }
	template<typename... Args> void warn(std::string_view a_format, Args&&... a_args) { core::logger_instances::get_core()->warn(a_format, std::forward<Args>(a_args)...); }
	template<typename... Args> void error(std::string_view a_format, Args&&... a_args) { core::logger_instances::get_core()->error(a_format, std::forward<Args>(a_args)...); }
	template<typename... Args> void fatal(std::string_view a_format, Args&&... a_args) { core::logger_instances::get_core()->critical(a_format, std::forward<Args>(a_args)...); }
}