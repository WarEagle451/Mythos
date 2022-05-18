#pragma once
#include <mythos/core/core.hpp>
#include <mythos/core/log.hpp> // MYL_CORE_FATAL()

#include <filesystem>

#ifdef MYL_BUILD_DEBUG
#	define MYL_ENABLE_ASSERTS
#	if _MSC_VER
#		define MYL_DEBUGBREAK() __debugbreak()
#	else
#		define MYL_DEBUGBREAK() __builtin_trap()
#	endif
#else
#	define MYL_DEBUGBREAK()
#endif

#ifdef MYL_ENABLE_ASSERTS
#	define MYL_ASSERT_IMPL(output, check, msg, ...) { if(!(check)) { output(msg, __VA_ARGS__); MYL_DEBUGBREAK(); } }

#	define MYL_INTERNAL_ASSERT_MSG(check, ...) MYL_ASSERT_IMPL(MYL_CORE_FATAL, check, "Assertion failed: {0}", __VA_ARGS__)
#	define MYL_INTERNAL_ASSERT_NO_MSG(check) MYL_ASSERT_IMPL(MYL_CORE_FATAL, check, "Assertion '{0}' failed at {1}:{2}", #check, std::filesystem::path(__FILE__).filename().string(), __LINE__)

#	define MYL_INTERNAL_ASSERT_GET_MACRO_NAME(check, arg, macro, ...) macro
#	define MYL_INTERNAL_ASSERT_GET_MACRO(...) MYL_EXPAND_MACRO(MYL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MYL_INTERNAL_ASSERT_MSG, MYL_INTERNAL_ASSERT_NO_MSG))

#	define MYL_CORE_ASSERT(...) MYL_EXPAND_MACRO(MYL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
#	define MYL_CORE_ASSERT(...)
#endif