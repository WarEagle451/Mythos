#pragma once
#include <mythos/log.hpp>

namespace editor {
    class log {
        static std::shared_ptr<spdlog::logger> s_logger;
    public:
        static auto init() -> void;
        static auto get() noexcept -> std::shared_ptr<spdlog::logger>;
    };
}

#ifdef MYL_DEBUG
#   define EDITOR_DEBUG(...) ::myth::log::get()->debug(__VA_ARGS__)
#else
#   define EDITOR_DEBUG(...)
#endif
#define EDITOR_TRACE(...) ::editor::log::get()->trace(__VA_ARGS__)
#define EDITOR_INFO(...) ::editor::log::get()->info(__VA_ARGS__)
#define EDITOR_WARN(...) ::editor::log::get()->warn(__VA_ARGS__)
#define EDITOR_ERROR(...) ::editor::log::get()->error(__VA_ARGS__)
#define EDITOR_FATAL(...) ::editor::log::get()->critical(__VA_ARGS__)
