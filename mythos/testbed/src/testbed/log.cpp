#include <testbed/log.hpp>

#include <spdlog/sinks/ansicolor_sink.h>

#include <vector>

namespace testbed {
    std::shared_ptr<spdlog::logger> log::s_logger = nullptr;

    auto log::init() -> void {
        std::vector<spdlog::sink_ptr> sinks;
        auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
        color_sink->set_color(spdlog::level::trace, mye::log::color(spdlog::level::trace));
        color_sink->set_color(spdlog::level::debug, mye::log::color(spdlog::level::debug));
        color_sink->set_color(spdlog::level::info, mye::log::color(spdlog::level::info));
        color_sink->set_color(spdlog::level::warn, mye::log::color(spdlog::level::warn));
        color_sink->set_color(spdlog::level::err, mye::log::color(spdlog::level::err));
        color_sink->set_color(spdlog::level::critical, mye::log::color(spdlog::level::critical));

        sinks.emplace_back(color_sink);
        sinks.back()->set_pattern("%^[%L] %n: %v%$");

        s_logger = std::make_shared<spdlog::logger>("Testbed", sinks.begin(), sinks.end());
        s_logger->set_level(spdlog::level::trace);
        spdlog::register_logger(s_logger);
    }

    auto log::get() noexcept -> std::shared_ptr<spdlog::logger> {
        return s_logger;
    }
}
