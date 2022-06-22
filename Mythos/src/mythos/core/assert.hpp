#pragma once
#include "log.hpp"
#include "except.hpp"

#include <filesystem>

#ifdef MYL_BUILD_DEBUG
#	if MYL_COMPILER_MSVC
#		define MYL_DEBUGBREAK() __debugbreak()
#	else
#		define MYL_DEBUGBREAK() __builtin_trap()
#	endif
#else
#	define MYL_DEBUGBREAK()
#endif

/// MYTodo: Use source location
#define MYL_MAKE_ASSERT(condition, output, msg, ...) { if (!(condition)) MYL_UNLIKELY { output(msg, __VA_ARGS__); MYL_DEBUGBREAK(); } }
#if !defined(MYL_DISABLE_ASSERTS) && (defined(MYL_ENABLE_ASSERTS) || defined(MYL_BUILD_DEBUG))
#	define MYL_INTERNAL_ASSERT_MSG(check, ...) MYL_MAKE_ASSERT(check, MYL_CORE_FATAL, "Assertion failed: {0}", __VA_ARGS__)
#	define MYL_INTERNAL_ASSERT_NO_MSG(check) MYL_MAKE_ASSERT(check, MYL_CORE_FATAL, "Assertion '{0}' failed at {1}:{2}", #check, std::filesystem::path(__FILE__).filename().string(), __LINE__)
#	define MYL_INTERNAL_ASSERT_GET_MACRO_NAME(check, arg, macro, ...) macro
#	define MYL_INTERNAL_ASSERT_GET_MACRO(...) MYL_EXPAND_MACRO(MYL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MYL_INTERNAL_ASSERT_MSG, MYL_INTERNAL_ASSERT_NO_MSG))

	/// MYTodo: Find a way to allow to pass thorugh args and also use it as a formater
#	define MYL_CORE_ASSERT(...) MYL_EXPAND_MACRO(MYL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
#	define MYL_CORE_ASSERT(...)
#endif

#define MYL_MAKE_VERIFY(condition, exception, ...) { if (!(condition)) MYL_UNLIKELY throw exception(__VA_ARGS__); }
#ifndef MYL_DISABLE_VERIFIES
#	define MYL_CORE_VERIFY(condition, ...) MYL_MAKE_VERIFY(condition, myl::core_exception, std::source_location::current(), __VA_ARGS__)
#else
#	define MYL_CORE_VERIFY(condition, ...)
#endif