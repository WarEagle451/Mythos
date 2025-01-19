#pragma once
#include <mythos/log.hpp>

#include <myl/definitions.hpp>
#include <myl/macro.hpp>

#include <format>
#include <source_location>

#define MYTHOS_MAKE_ASSERT(log, source, condition, ...) {\
    if (!(condition)) {\
        MYL_IF(MYL_VA_EMPTY(__VA_ARGS__), \
            log("{} - {}: Assertion '{}' failed at {}:{}", source.file_name(), source.function_name(), MYL_STRINGIFY(condition), source.line(), source.column()),\
            MYL_IF(MYL_VA_EMPTY(MYL_ARG_DISCARD(__VA_ARGS__)),\
                log("{} - {}: Assertion failed at {}:{} - {}", source.file_name(), source.function_name(), source.line(), source.column(), __VA_ARGS__),\
                log("{} - {}: Assertion failed at {}:{} - {}", source.file_name(), source.function_name(), source.line(), source.column(), std::format(__VA_ARGS__))\
            )\
        );\
        MYL_DEBUG_BREAK;\
    }\
}

#ifdef MYL_DEBUG
#   define MYTHOS_ASSERT(...) MYTHOS_MAKE_ASSERT(MYTHOS_FATAL, std::source_location::current(), __VA_ARGS__)
#else
#   define MYTHOS_ASSERT(...)
#endif

#define MYTHOS_VERIFY(...) MYTHOS_MAKE_ASSERT(MYTHOS_FATAL, std::source_location::current(), __VA_ARGS__)
