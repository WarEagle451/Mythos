#pragma once
#include <mythos/platform/detection.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <errhandlingapi.h>

#	include <string>

namespace myl::windows {
	static std::string GetLastError_as_string() {
		DWORD error_id = GetLastError();
		if (error_id == 0)
			return std::string(); // No error

		LPSTR message_buf = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									 nullptr, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&message_buf), 0, nullptr);

		std::string message(message_buf, size);
		LocalFree(message_buf);
		return message;
	}
}
#endif