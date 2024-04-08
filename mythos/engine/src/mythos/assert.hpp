#pragma once
#include <mythos/log.hpp>

#include <myl/definitions.hpp>
#include <myl/macro.hpp>

#include <source_location>

#define MYTHOS_MAKE_ASSERT(log, condition, ...) {\
    if (!(condition)) {\
        constexpr auto sl = std::source_location::current();\
        if constexpr (MYL_VA_EMPTY(__VA_ARGS__))\
            log("{} - {}: Assertion '{}' failed at {}:{}", sl.file_name(), sl.function_name(), MYL_STRINGIFY(condition), sl.line(), sl.column());\
        else\
            log("{} - {}: Assertion failed at {}:{} - {}", sl.file_name(), sl.function_name(), sl.line(), sl.column(), MYL_ARG_1(__VA_ARGS__ ""));\
        MYL_DEBUG_BREAK;\
    }\
}

#ifdef MYL_DEBUG
#   define MYTHOS_ASSERT(...) MYTHOS_MAKE_ASSERT(MYTHOS_FATAL, __VA_ARGS__)
#else
#   define MYTHOS_ASSERT(...)
#endif

#define MYTHOS_VERIFY(...) MYTHOS_MAKE_ASSERT(MYTHOS_FATAL, __VA_ARGS__)
