#pragma once
#include <mythos/defines.hpp>

#include <string>

namespace myl {
	struct core_exception {
	protected:
		std::string m_message;
	public:
		MYL_API core_exception() noexcept
			: m_message("") {}
		MYL_API explicit core_exception(std::string_view a_message)
			: m_message(a_message) {}
		MYL_API explicit core_exception(std::string_view a_prefix, std::string_view a_message)
			: m_message(a_prefix) { m_message.append(a_message); }

		MYL_API MYL_NO_DISCARD const char* message() const { return m_message.empty() ? "Unknown exception" : m_message.data(); }
	};

	struct core_runtime_error : public core_exception {
		MYL_API explicit core_runtime_error(std::string_view a_message)
			: core_exception("Core runtime error - ", a_message) {}
	};
}