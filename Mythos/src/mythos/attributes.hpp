#pragma once
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