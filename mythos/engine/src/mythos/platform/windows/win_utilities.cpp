#include <mythos/platform/windows/win_utilities.hpp>
#ifdef MYL_OS_WINDOWS
#   include <winbase.h>
#   include <stringapiset.h>

namespace myth::win {
    MYL_NO_DISCARD auto last_error_as_string(DWORD code) -> std::string {
        LPTSTR message_buffer = NULL;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&message_buffer,
            0,
            NULL
        );

        if (message_buffer == NULL)
            return "";

#ifdef UNICODE
        int wlen = WideCharToMultiByte(CP_ACP, 0, message_buffer, -1, 0, 0, NULL, NULL);
        std::string message;
        message.reserve(wlen);
        WideCharToMultiByte(CP_ACP, 0, message_buffer, -1, &message[0], wlen, NULL, NULL);
#else
        std::string message(message_buffer);
#endif
        LocalFree(message_buffer);
        return message;
    }
}
#endif
