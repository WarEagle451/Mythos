#include <mythos/core/log.hpp>

#include <spdlog/sinks/ansicolor_sink.h>

#include <vector>

namespace myth {
    std::shared_ptr<spdlog::logger> log::s_logger = nullptr;

    auto log::init() -> void {
        std::vector<spdlog::sink_ptr> sinks;
        auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
		color_sink->set_color(spdlog::level::trace, "\033[38;2;230;230;230m");
		color_sink->set_color(spdlog::level::debug, "\033[38;2;240;130;20m");
		color_sink->set_color(spdlog::level::info, "\033[38;2;50;150;40m");
		color_sink->set_color(spdlog::level::warn, "\033[38;2;230;185;50m");
		color_sink->set_color(spdlog::level::err, "\033[38;2;200;50;50m");
		color_sink->set_color(spdlog::level::critical, "\033[38;2;0;0;0;48;2;200;50;50m");

		sinks.emplace_back(color_sink);
		sinks.back()->set_pattern("%^[%L]: %v%$");

        s_logger = std::make_shared<spdlog::logger>("Mythos", sinks.begin(), sinks.end());
        s_logger->set_level(spdlog::level::trace);
        spdlog::register_logger(s_logger);
    }

    auto log::get() noexcept -> std::shared_ptr<spdlog::logger> {
        return s_logger;
    }
}
