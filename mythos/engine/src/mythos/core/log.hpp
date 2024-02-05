#pragma once
#define SPDLOG_LEVEL_NAMES { "Trace", "Debug", "Info", "Warning", "Error", "Fatal", "Off" }
#define SPDLOG_SHORT_LEVEL_NAMES { "T", "D", "I", "W", "E", "F", "O" }
#include <spdlog/spdlog.h>

#include <memory>

namespace myth {
    class log {
        static std::shared_ptr<spdlog::logger> s_logger;
    public:
        static auto init() -> void;
        static auto get() noexcept -> std::shared_ptr<spdlog::logger>;
    };
}

#include <myl/definitions.hpp>
#ifdef MYL_DEBUG
#   define MYTHOS_DEBUG(...) ::myth::log::get()->debug(__VA_ARGS__)
#else
#   define MYTHOS_DEBUG(...)
#endif
#define MYTHOS_TRACE(...) ::myth::log::get()->trace(__VA_ARGS__)
#define MYTHOS_INFO(...) ::myth::log::get()->info(__VA_ARGS__)
#define MYTHOS_WARN(...) ::myth::log::get()->warn(__VA_ARGS__)
#define MYTHOS_ERROR(...) ::myth::log::get()->error(__VA_ARGS__)
#define MYTHOS_FATAL(...) ::myth::log::get()->critical(__VA_ARGS__)
