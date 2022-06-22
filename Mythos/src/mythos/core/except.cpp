#include "except.hpp"

#include <format>

namespace myl {
	static std::string format_source_location(const std::source_location& source) {
		return std::format("'{}' in '{}' @ [L: {}, C: {}]",
			source.file_name(),
			source.function_name(),
			source.line(),
			source.column());
	}

	core_exception::core_exception() noexcept
		: m_message("") {}

	core_exception::core_exception(std::string_view a_message)
		: m_message(a_message) {}

	core_exception::core_exception(std::string_view a_prefix, std::string_view a_message)
		: m_message(a_prefix) {
		m_message.append(a_message);
	}

	core_exception::core_exception(const std::source_location& a_source)
		: m_message("An exception occurred: " + format_source_location(a_source)) {}

	core_exception::core_exception(const std::source_location& a_source, std::string_view a_message)
		: m_message(a_message) {
		m_message.append(" - " + format_source_location(a_source));
	}

	core_exception::core_exception(const std::source_location& a_source, std::string_view a_prefix, std::string_view a_message)
		: m_message(a_prefix.data()) {
		m_message.append(a_message);
		m_message.append(" - " + format_source_location(a_source));
	}

	MYL_NO_DISCARD const char* core_exception::message() const {
		return m_message.empty() ? "An exception occurred" : m_message.data();
	}

	core_runtime_error::core_runtime_error(std::string_view a_message)
		: core_exception("Core runtime error - ", a_message) {}

	core_runtime_error::core_runtime_error(const std::source_location& a_source)
		: core_exception(format_source_location(a_source)) {}

	core_runtime_error::core_runtime_error(const std::source_location& a_source, std::string_view a_message)
		: core_exception(a_source, "Core runtime error - ", a_message) {}
}