#pragma once
#include "defines.hpp"

#include <string>

/// MYTodo: Add ways to get signifigate data from the uuid types, ie low time, mac address

namespace myl {
	enum class uuid_version {
		unknown		= 0x00,
		time		= 0x10,
		dce			= 0x20,
		name_md5	= 0x30,
		random		= 0x40,
		name_sha1	= 0x50
	};

	enum class uuid_variant {
		unknown,
		ncs,
		rfc_4122,
		microsoft,
		reserved
	};

	class uuid { /// MYTodo: can do clas MYL_API uuid instead of all inside?
		u8 m_bytes[16]{}; // 128 bits
	public:
		MYL_API MYL_NO_DISCARD constexpr uuid() noexcept;
		MYL_API MYL_NO_DISCARD uuid(uuid_version);
		MYL_API MYL_NO_DISCARD constexpr uuid(u8*);
		MYL_API MYL_NO_DISCARD constexpr uuid(std::string_view);

		MYL_API MYL_NO_DISCARD constexpr u8* data();
		MYL_API MYL_NO_DISCARD constexpr const u8* data() const;

		MYL_API MYL_NO_DISCARD bool nil() const;
		MYL_API MYL_NO_DISCARD constexpr uuid_version version() const;
		MYL_API MYL_NO_DISCARD constexpr uuid_variant variant() const;
		MYL_API MYL_NO_DISCARD constexpr std::string string(bool a_brackets = false, bool a_dashed = true) const;

		MYL_API MYL_NO_DISCARD bool operator==(const uuid& rhs) const;
		MYL_API MYL_NO_DISCARD bool operator<(const uuid& rhs) const;
		MYL_API MYL_NO_DISCARD bool operator>(const uuid& rhs) const;
		MYL_API MYL_NO_DISCARD bool operator<=(const uuid& rhs) const { return !(*this > rhs); }
		MYL_API MYL_NO_DISCARD bool operator>=(const uuid& rhs) const { return !(*this < rhs); }
	};
}

static_assert(sizeof(myl::uuid) == 16, "Expected myl::uuid to be 16 bytes");