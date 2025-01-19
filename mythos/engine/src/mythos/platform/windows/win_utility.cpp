#include <mythos/platform/windows/win_utility.hpp>
#ifdef MYL_OS_WINDOWS
#   include <stringapiset.h>

namespace mye::win {
    MYL_NO_DISCARD auto error_as_string(DWORD code) -> std::string {
#ifdef UNICODE
        using buffer_t = LPTSTR;
#else
        using buffer_t = LPSTR;
#endif

        buffer_t buffer = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, 0, (buffer_t)&buffer, 0, NULL);

        if (buffer == NULL)
            return "";

#ifdef UNICODE
        const int length = WideCharToMultiByte(CP_ACP, 0, buffer, -1, 0, 0, NULL, NULL);
        std::string message(length, 0);
        WideCharToMultiByte(CP_ACP, 0, buffer, -1, &message[0], length, NULL, NULL);
#else
        std::string message(buffer);
#endif
        LocalFree(buffer);
        return message;
    }
}
#endif
