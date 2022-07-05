#pragma once
#include <mythos/defines.hpp>

#include <bit>

namespace myl {
	MYL_API MYL_NO_DISCARD inline consteval bool system_endian_big() { return std::endian::native == std::endian::big; }
	MYL_API MYL_NO_DISCARD inline consteval bool system_endian_little() { return std::endian::native == std::endian::little; }

	//@return Passed param as big endian
	MYL_API MYL_NO_DISCARD u16 hton16(u16);
	//@return Passed param as big endian
	MYL_API MYL_NO_DISCARD u32 hton32(u32);
	//@return Passed param as big endian
	MYL_API MYL_NO_DISCARD u64 hton64(u64);
	
	//@return Passed param as host native endian
	MYL_API MYL_NO_DISCARD u16 ntoh16(u16);
	//@return Passed param as host native endian
	MYL_API MYL_NO_DISCARD u32 ntoh32(u32);
	//@return Passed param as host native endian
	MYL_API MYL_NO_DISCARD u64 ntoh64(u64);
}