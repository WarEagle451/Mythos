#pragma once
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

using b32 = int;
using b8 = char;

#ifndef MYL_NO_RETURN
#	define MYL_NO_RETURN [[noreturn]]
#endif

#ifndef MYL_CARRIES_DEPENDENCY
#	define MYL_CARRIES_DEPENDENCY [[carries_dependency]]
#endif

#ifndef MYL_DEPRECATED
#	define MYL_DEPRECATED(reason) [[deprecated(reason)]]
#endif

#ifndef MYL_FALLTHROUGH
#	define MYL_FALLTHROUGH [[fallthrough]]
#endif

#ifndef MYL_NO_DISCARD
#	define MYL_NO_DISCARD [[nodiscard]]
#endif

#ifndef MYL_NO_DISCARD_MSG
#	define MYL_NO_DISCARD_MSG(msg) [[nodiscard(msg)]]
#endif

#ifndef MYL_MAYBE_UNUSED
#	define MYL_MAYBE_UNUSED [[maybe_unused]]
#endif

#ifndef MYL_NO_UNIQUE_ADDRESS
#	define MYL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#ifndef MYL_LIKELY
#	define MYL_LIKELY [[likely]]
#endif

#ifndef MYL_UNLIKELY
#	define MYL_UNLIKELY [[unlikely]]
#endif

#ifdef MYL_EXPORT
#	ifdef _MSC_VER
#		define MYL_API __declspec(dllexport)
#	else
#		define MYL_API __attribute__((visibility("default")))
#	endif
#else
#	ifdef _MSC_VER
#		define MYL_API __declspec(dllimport)
#	else
#		define MYL_API
#	endif
#endif

#if defined(__clang__) || defined(__gcc__)
#	define MYL_STATIC_ASSERT _Static_assert
#else
#	define MYL_STATIC_ASSERT static_assert
#endif

MYL_STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte");
MYL_STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes");
MYL_STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes");

MYL_STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte");
MYL_STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes");
MYL_STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes");

MYL_STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes");

MYL_STATIC_ASSERT(sizeof(b32) == 4, "Expected b32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(b8) == 1, "Expected b8 to be 1 byte");