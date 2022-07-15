#include "random.hpp"
#include "uuid.hpp"

#include <mythos/network/endian.hpp>

#include <atomic>
#include <chrono>

/// Sections to review
/// 4.3
/// 

namespace myl {
	static const char g_uuid_digits[16]{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	MYL_NO_DISCARD static constexpr u8 char_to_hex(char c) {
		switch (c) {
			case '0': return 0x00;
			case '1': return 0x01;
			case '2': return 0x02;
			case '3': return 0x03;
			case '4': return 0x04;
			case '5': return 0x05;
			case '6': return 0x06;
			case '7': return 0x07;
			case '8': return 0x08;
			case '9': return 0x09;
			case 'A': MYL_FALLTHROUGH;
			case 'a': return 0x0A;
			case 'B': MYL_FALLTHROUGH;
			case 'b': return 0x0B;
			case 'C': MYL_FALLTHROUGH;
			case 'c': return 0x0C;
			case 'D': MYL_FALLTHROUGH;
			case 'd': return 0x0D;
			case 'E': MYL_FALLTHROUGH;
			case 'e': return 0x0E;
			case 'F': MYL_FALLTHROUGH;
			case 'f': return 0x0F;
			default: return 0xFF;
		}
	}

	static void assign_time_uuid(u8(&bytes)[16]) { // 4.2
		static constexpr const u64 ns_intervals_15_oct_1582_to_jan_1_1970 = 122'192'928'000'000'000; // Magic number! (141427 days in 100ns intervals)
		static generator_u64 node = generator_u64();
		/// MYTodo: Clock sequence is supposed to be corrected to section 4.5
		static std::atomic<u16> clock_sequence = generator_u16()(); // 4.1.5

		const u64 ns_intervals_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::utc_clock::now().time_since_epoch()).count() / 100; // RFC states that the time is in 100 nanosecond intervals
		const u64 intervals = ns_intervals_15_oct_1582_to_jan_1_1970 + ns_intervals_since_epoch;
		u64* as_u64 = reinterpret_cast<u64*>(bytes);

		as_u64[0] = // Low time is ahead by 27 seconds, reason: https://en.wikipedia.org/wiki/Leap_second#Insertion_of_leap_seconds
			(intervals & 0x0000'0000'FFFF'FFFFull) << 32 |	// Low 32 bits
			(intervals & 0x0000'FFFF'0000'0000ull) >> 16 |	// Mid 16 bits
			(intervals & 0xFFFF'0000'0000'0000ull) >> 48;	// High 16 bits
		as_u64[1] =
			static_cast<u64>(clock_sequence) << 48 |	// 16 bits
			node() >> 16;								// 48 bits and does not use MAC address for security reasons

		as_u64[0] = hton64(as_u64[0]); // Ensures data is stored as big endian
		as_u64[1] = hton64(as_u64[1]); // Ensures data is stored as big endian

		bytes[10] |= 0x01; // If MAC address is replaced with random number, the RFC requires that the least significant bit of the first octet of the node ID should be set to 1

		bytes[6] &= 0x1F; bytes[6] |= 0x10; // Version time
		bytes[8] &= 0xBF; bytes[8] |= 0x80; // Variant rfc_4122

		++clock_sequence; /// MYTodo: 4.2.1, bullet 6. Should not be doing this
	}

	static constexpr void assign_dce_uuid(u8(&bytes)[16]) {
		/// MYTodo: UUID gen
	}

	static constexpr void assign_md5_uuid(u8(&bytes)[16]) {
		/// MYTodo: UUID gen
	}

	static void assign_random_uuid(u8(&bytes)[16]) { // 4.4
		u64* as_u64 = reinterpret_cast<u64*>(bytes); // This is okay because endianess will further randomize the bits
		generator_u64 gen = generator_u64();

		as_u64[0] = gen();
		as_u64[1] = gen();

		bytes[6] &= 0x4F; bytes[6] |= 0x40; // Version random
		bytes[8] &= 0xBF; bytes[8] |= 0x80; // Variant rfc_4122
	}

	static constexpr void assign_sha1_uuid(u8(&bytes)[16]) {
		/// MYTodo: UUID gen
	}


	uuid::uuid(uuid_version a_version) {
		switch (a_version) {
			using enum uuid_version;
			case time:		assign_time_uuid(m_bytes);		break;
			case dce:		assign_dce_uuid(m_bytes);		break;
			case md5:	assign_md5_uuid(m_bytes);	break;
			case random:	assign_random_uuid(m_bytes);	break;
			case sha1: assign_sha1_uuid(m_bytes);	break;
			default: break; // Default initializes to nil
		}
	}

	constexpr uuid::uuid(u8(&a_bytes)[16])
		: m_bytes{
		a_bytes[0], a_bytes[1], a_bytes[2], a_bytes[3],
		a_bytes[4], a_bytes[5],
		a_bytes[6], a_bytes[7],
		a_bytes[8], a_bytes[9],
		a_bytes[10], a_bytes[11], a_bytes[12], a_bytes[13], a_bytes[14], a_bytes[15] } {}

	constexpr uuid::uuid(std::string_view a_view) // RFC 4122 Section 3 requires input to be case insensitive
		: m_bytes{
		char_to_hex(a_view[0]), char_to_hex(a_view[1]), char_to_hex(a_view[2]), char_to_hex(a_view[3]),
		char_to_hex(a_view[4]), char_to_hex(a_view[5]),
		char_to_hex(a_view[6]), char_to_hex(a_view[7]),
		char_to_hex(a_view[8]), char_to_hex(a_view[9]),
		char_to_hex(a_view[10]), char_to_hex(a_view[11]), char_to_hex(a_view[12]), char_to_hex(a_view[13]), char_to_hex(a_view[14]), char_to_hex(a_view[15]) } {}

	constexpr u8* uuid::data() {
		return m_bytes;
	}

	constexpr const u8* uuid::data() const {
		return m_bytes;
	}

	constexpr uuid_version uuid::version() const { // Section 4.1.3
		switch (m_bytes[6] & 0xF0) {
			using enum uuid_version;
			case 0x10: return time;
			case 0x20: return dce;
			case 0x30: return md5;
			case 0x40: return random;
			case 0x50: return sha1;
			default: return unknown;
		}
	}

	constexpr uuid_variant uuid::variant() const { // Section 4.1.1
		const u8 byte = m_bytes[8];
		using enum uuid_variant;
		if ((byte & 0x80) == 0x00)		return ncs;
		else if ((byte & 0xC0) == 0x80) return rfc_4122;
		else if ((byte & 0xE0) == 0xC0) return microsoft;
		else if ((byte & 0xE0) == 0xE0) return reserved;
		else							return unknown;
	}

	bool uuid::nil() const { // 4.1.7
		const u64* as_u64 = reinterpret_cast<const u64*>(m_bytes);
		return as_u64[0] == 0 && as_u64[1] == 0;
	}

	constexpr std::string uuid::string(bool a_brackets, bool a_dashed) const { // RFC 4122 Section 3 requires output to be lowercase
		std::string out;
		out.reserve(32 + (a_brackets ? 2 : 0) + (a_dashed ? 4 : 0));
		if (a_brackets) out += '{';
		for (u32 i = 0; i != 16; ++i) {
			if (a_dashed && (i == 4 || i == 6 || i == 8 || i == 10))
				out += '-';
			out += g_uuid_digits[m_bytes[i] >> 4];
			out += g_uuid_digits[m_bytes[i] & 0x0F];
		}
		if (a_brackets) out += '}';
		return out;
	}

	bool uuid::operator==(const uuid& rhs) const { // reinterpret_cast can be used in comparision operations
		const u64* lhs_u64 = reinterpret_cast<const u64*>(m_bytes);
		const u64* rhs_u64 = reinterpret_cast<const u64*>(rhs.m_bytes);
		return lhs_u64[0] == rhs_u64[0] && lhs_u64[1] == rhs_u64[1];
	}

	bool uuid::operator<(const uuid& rhs) const { // reinterpret_cast can be used in comparision operations
		const u64* lhs_u64 = reinterpret_cast<const u64*>(m_bytes);
		const u64* rhs_u64 = reinterpret_cast<const u64*>(rhs.m_bytes);
		return lhs_u64[0] < rhs_u64[0] ||
			(lhs_u64[0] == rhs_u64[0] && lhs_u64[1] < rhs_u64[1]);
	}

	bool uuid::operator>(const uuid& rhs) const { // reinterpret_cast can be used in comparision operations
		const u64* lhs_u64 = reinterpret_cast<const u64*>(m_bytes);
		const u64* rhs_u64 = reinterpret_cast<const u64*>(rhs.m_bytes);
		return lhs_u64[0] > rhs_u64[0] ||
			(lhs_u64[0] == rhs_u64[0] && lhs_u64[1] > rhs_u64[1]);
	}
}