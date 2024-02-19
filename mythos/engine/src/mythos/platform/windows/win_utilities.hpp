#pragma once
#include <myl/definitions.hpp>
#ifdef MYL_OS_WINDOWS
#   include <windows.h>

#   include <string>

namespace myth::win {
    MYL_NO_DISCARD auto last_error_as_string(DWORD code) -> std::string;
}
#endif
