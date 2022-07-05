#include "endian.hpp"

#include <mythos/platform/detection.hpp>

#define MYL_ENDIAN_CONVERT_LOGIC(conversion_func_, value_) {\
	if constexpr (myl::system_endian_big())\
		return value_;\
	else return conversion_func_(value_);\
}

#ifdef MYL_PLATFORM_WINDOWS
#	include <WinSock2.h>
#	include <windows.h>

namespace myl {
	u16 hton16(u16 value) { MYL_ENDIAN_CONVERT_LOGIC(htons, value); }
	u32 hton32(u32 value) { MYL_ENDIAN_CONVERT_LOGIC(htonl, value); }
	u64 hton64(u64 value) { MYL_ENDIAN_CONVERT_LOGIC(htonll, value); }
	
	u16 ntoh16(u16 value) { MYL_ENDIAN_CONVERT_LOGIC(ntohs, value); }
	u32 ntoh32(u32 value) { MYL_ENDIAN_CONVERT_LOGIC(ntohl, value); }
	u64 ntoh64(u64 value) { MYL_ENDIAN_CONVERT_LOGIC(ntohll, value); }
}
#else
#	error Unknown platform
#endif