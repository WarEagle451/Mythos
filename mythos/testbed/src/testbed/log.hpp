#pragma once
#include <mythos/log.hpp>

namespace testbed {
    class log {
        static std::shared_ptr<spdlog::logger> s_logger;
    public:
        static auto init() -> void;
        static auto get() noexcept -> std::shared_ptr<spdlog::logger>;
    };
}

#ifdef MYL_DEBUG
#   define TESTBED_DEBUG(...) ::testbed::log::get()->debug(__VA_ARGS__)
#else
#   define TESTBED_DEBUG(...)
#endif
#define TESTBED_TRACE(...) ::testbed::log::get()->trace(__VA_ARGS__)
#define TESTBED_INFO(...) ::testbed::log::get()->info(__VA_ARGS__)
#define TESTBED_WARN(...) ::testbed::log::get()->warn(__VA_ARGS__)
#define TESTBED_ERROR(...) ::testbed::log::get()->error(__VA_ARGS__)
#define TESTBED_FATAL(...) ::testbed::log::get()->critical(__VA_ARGS__)
