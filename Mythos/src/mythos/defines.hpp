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

#define MYL_NO_RETURN [[noreturn]]
#define MYL_CARRIES_DEPENDENCY [[carries_dependency]]
#define MYL_DEPRECATED(reason) [[deprecated(reason)]]
#define MYL_FALLTHROUGH [[fallthrough]]
#define MYL_NO_DISCARD [[nodiscard]]
#define MYL_NO_DISCARD_MSG(msg) [[nodiscard(msg)]]
#define MYL_MAYBE_UNUSED [[maybe_unused]]
#define MYL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#define MYL_LIKELY [[likely]]
#define MYL_UNLIKELY [[unlikely]]

#define MYL_VERSION_MAJOR 0
#define MYL_VERSION_MINOR 0
#define MYL_VERSION_PATCH 0
#define MYL_VERSION (MYL_VERSION_MAJOR * 1000000 + MYL_VERSION_MINOR * 1000 + MYL_VERSION_PATCH)

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

MYL_STATIC_ASSERT(sizeof(u8) == 1, "expected u8 to be 1 byte");
MYL_STATIC_ASSERT(sizeof(u16) == 2, "expected u16 to be 2 bytes");
MYL_STATIC_ASSERT(sizeof(u32) == 4, "expected u32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(u64) == 8, "expected u64 to be 8 bytes");

MYL_STATIC_ASSERT(sizeof(i8) == 1, "expected i8 to be 1 byte");
MYL_STATIC_ASSERT(sizeof(i16) == 2, "expected i16 to be 2 bytes");
MYL_STATIC_ASSERT(sizeof(i32) == 4, "expected i32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(i64) == 8, "expected i64 to be 8 bytes");

MYL_STATIC_ASSERT(sizeof(f32) == 4, "expected f32 to be 4 bytes");
MYL_STATIC_ASSERT(sizeof(f64) == 8, "expected f64 to be 8 bytes");