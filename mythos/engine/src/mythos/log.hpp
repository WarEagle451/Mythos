#pragma once
#include <myl/definitions.hpp>

#define SPDLOG_LEVEL_NAMES { "Trace", "Debug", "Info", "Warning", "Error", "Fatal", "Off" }
#define SPDLOG_SHORT_LEVEL_NAMES { "T", "D", "I", "W", "E", "F", "O" }
#include <spdlog/spdlog.h>

#include <memory>

/// MYTODO: Change format to [Time] [name] [severity]: [message] - File: [file], Line: [line], Index: [index]
/// only for error and fatal

namespace mye {
    class log {
        static std::shared_ptr<spdlog::logger> s_logger;
    public:
        static auto init() -> void;
        static auto get() noexcept -> std::shared_ptr<spdlog::logger>;

        MYL_NO_DISCARD static auto color(spdlog::level::level_enum level) -> const char*;
    };
}

#ifdef MYL_DEBUG
#   define MYTHOS_DEBUG(...) ::mye::log::get()->debug(__VA_ARGS__)
#else
#   define MYTHOS_DEBUG(...)
#endif

#define MYTHOS_TRACE(...) ::mye::log::get()->trace(__VA_ARGS__)
#define MYTHOS_INFO(...) ::mye::log::get()->info(__VA_ARGS__)
#define MYTHOS_WARN(...) ::mye::log::get()->warn(__VA_ARGS__)
#define MYTHOS_ERROR(...) ::mye::log::get()->error(__VA_ARGS__)
#define MYTHOS_FATAL(...) ::mye::log::get()->critical(__VA_ARGS__)
