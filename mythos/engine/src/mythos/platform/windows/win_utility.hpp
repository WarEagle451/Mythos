#pragma once
#include <myl/definitions.hpp>
#ifdef MYL_OS_WINDOWS
#   include <Windows.h>

#   include <string>

namespace myth::win {
    MYL_NO_DISCARD auto error_as_string(DWORD code) -> std::string;

    MYL_NO_DISCARD inline auto last_error_as_string() -> std::string {
        return error_as_string(GetLastError());
    }
}

#endif
