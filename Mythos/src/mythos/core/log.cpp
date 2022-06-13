#include "log.hpp"

#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <vector>

namespace myl {
	std::shared_ptr<spdlog::logger> loggers::s_core = nullptr;

	void loggers::init() {
		std::vector<spdlog::sink_ptr> sinks;
#ifdef MYL_BUILD_DEBUG
		auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
		color_sink->set_color(spdlog::level::trace, "\033[38;2;230;230;230m");
		color_sink->set_color(spdlog::level::debug, "\033[38;2;240;130;20m");
		color_sink->set_color(spdlog::level::info, "\033[38;2;50;150;40m");
		color_sink->set_color(spdlog::level::warn, "\033[38;2;230;185;50m");
		color_sink->set_color(spdlog::level::err, "\033[38;2;200;50;50m");
		color_sink->set_color(spdlog::level::critical, "\033[38;2;0;0;0;48;2;200;50;50m");

		sinks.emplace_back(color_sink);
		sinks.back()->set_pattern("%^[%T] %n: %v%$");
#endif
		sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("mythos.log", true));
		sinks.back()->set_pattern("[%T] [%L] %n: %v");

		s_core = std::make_shared<spdlog::logger>("Mythos", sinks.begin(), sinks.end());
		spdlog::register_logger(s_core);

		s_core->flush_on(spdlog::level::trace);
#ifdef MYL_BUILD_DEBUG
		s_core->set_level(spdlog::level::trace);
#else
		s_core->set_level(spdlog::level::info);	
#endif
	}
}