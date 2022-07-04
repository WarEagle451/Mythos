#pragma once
#define MYL_EXPAND_MACRO(x) x
#define MYL_STRINGIFY(x) #x
#define MYL_STRINGIFY_ARG(arg) MYL_STRINGIFY(arg)

#define MYL_VERSION_MAJOR 0
#define MYL_VERSION_MINOR 0
#define MYL_VERSION_PATCH 0

#define MYL_MAKE_VERSION(major, minor, patch) (major * 10000 + minor * 100 + patch)
#define MYL_MAKE_VERSION_STRING(major, minor, patch) (MYL_STRINGIFY_ARG(major)##"."##MYL_STRINGIFY_ARG(minor)##"."##MYL_STRINGIFY_ARG(patch))

#define MYL_VERSION MYL_MAKE_VERSION(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH)
#define MYL_VERSION_STRING MYL_MAKE_VERSION_STRING(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH)

#define MYL_ENGINE_NAME "Mythos Engine"

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

#include "platform/detection.hpp"
#ifdef MYL_EXPORT
#	ifdef MYL_COMPILER_MSVC
#		define MYL_API __declspec(dllexport)
#	else
#		define MYL_API __attribute__((visibility("default")))
#	endif
#else
#	ifdef MYL_COMPILER_MSVC
#		define MYL_API __declspec(dllimport)
#	else
#		define MYL_API
#	endif
#endif

namespace myl {
	using u8	= unsigned char;
	using u16	= unsigned short;
	using u32	= unsigned int;
	using u64	= unsigned long long;

	using i8	= signed char;
	using i16	= signed short;
	using i32	= signed int;
	using i64	= signed long long;

	using f32	= float;
	using f64	= double;
}

static_assert(sizeof(myl::u8) == 1, "Expected u8 to be 1 byte");
static_assert(sizeof(myl::u16) == 2, "Expected u16 to be 2 bytes");
static_assert(sizeof(myl::u32) == 4, "Expected u32 to be 4 bytes");
static_assert(sizeof(myl::u64) == 8, "Expected u64 to be 8 bytes");

static_assert(sizeof(myl::i8) == 1, "Expected i8 to be 1 byte");
static_assert(sizeof(myl::i16) == 2, "Expected i16 to be 2 bytes");
static_assert(sizeof(myl::i32) == 4, "Expected i32 to be 4 bytes");
static_assert(sizeof(myl::i64) == 8, "Expected i64 to be 8 bytes");

static_assert(sizeof(myl::f32) == 4, "Expected f32 to be 4 bytes");
static_assert(sizeof(myl::f64) == 8, "Expected f64 to be 8 bytes");