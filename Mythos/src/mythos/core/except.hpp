#pragma once
#include <mythos/defines.hpp>

#include <string>
#include <source_location>

namespace myl {
	struct core_exception {
	protected:
		std::string m_message;
	public:
		MYL_API core_exception() noexcept;
		MYL_API explicit core_exception(std::string_view);
		MYL_API explicit core_exception(std::string_view a_prefix, std::string_view a_message);
		MYL_API explicit core_exception(const std::source_location&);
		MYL_API explicit core_exception(const std::source_location&, std::string_view);
		MYL_API explicit core_exception(const std::source_location&, std::string_view a_prefix, std::string_view a_message);

		MYL_API MYL_NO_DISCARD const char* message() const;
	};

	struct core_runtime_error : public core_exception {
		MYL_API explicit core_runtime_error(std::string_view);
		MYL_API explicit core_runtime_error(const std::source_location&);
		MYL_API explicit core_runtime_error(const std::source_location&, std::string_view);
	};
}