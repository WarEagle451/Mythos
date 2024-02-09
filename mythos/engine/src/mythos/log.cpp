#include <mythos/log.hpp>

#include <spdlog/sinks/ansicolor_sink.h>

#include <vector>

namespace myth {
    std::shared_ptr<spdlog::logger> log::s_logger = nullptr;

    auto log::init() -> void {
        std::vector<spdlog::sink_ptr> sinks;
        auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
		color_sink->set_color(spdlog::level::trace, log::color(spdlog::level::trace));
		color_sink->set_color(spdlog::level::debug, log::color(spdlog::level::debug));
		color_sink->set_color(spdlog::level::info, log::color(spdlog::level::info));
		color_sink->set_color(spdlog::level::warn, log::color(spdlog::level::warn));
		color_sink->set_color(spdlog::level::err, log::color(spdlog::level::err));
		color_sink->set_color(spdlog::level::critical, log::color(spdlog::level::critical));

		sinks.emplace_back(color_sink);
		sinks.back()->set_pattern("%^[%L] %n: %v%$");

        s_logger = std::make_shared<spdlog::logger>("Mythos", sinks.begin(), sinks.end());
        s_logger->set_level(spdlog::level::trace);
        spdlog::register_logger(s_logger);
    }

    auto log::get() noexcept -> std::shared_ptr<spdlog::logger> {
        return s_logger;
    }

    MYL_NO_DISCARD auto log::color(spdlog::level::level_enum level) -> const char* {
        switch (level) {
            case spdlog::level::trace:    return "\033[38;2;230;230;230m";
            case spdlog::level::debug:    return "\033[38;2;240;130;20m";
            case spdlog::level::info:     return "\033[38;2;50;150;40m";
            case spdlog::level::warn:     return "\033[38;2;230;185;50m";
            case spdlog::level::err:      return "\033[38;2;200;50;50m";
            case spdlog::level::critical: return "\033[38;2;0;0;0;48;2;200;50;50m";
            default:                      return "";
        }
    }
}
