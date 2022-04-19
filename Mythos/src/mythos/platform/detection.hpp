#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define MYL_PLATFORM_WINDOWS
#	ifndef _WIN64
#		error "64-bit is required on Windows"
#	endif
#elif defined(__APPLE__)
#	define MYL_PLATFORM_APPLE
#	include <TargetConditionals.h>
#	if TARGET_IPHONE_SIMULATOR
#		error "IOS simulator is not supported!"
#		define MYL_PLATFORM_IOS
#		define MYL_PLATFORM_IOS_SIMULATOR
#	elif TARGET_OS_IPHONE
#		error "IOS is not supported!"
#		define MYL_PLATFORM_IOS
#	elif TARGET_OS_MAC
#		error "MacOS is not supported!"
#		define MYL_PLATFORM_MAC_OS
#	else
#		error "Unknown Apple platform"
#	endif
#elif defined(__linux__) || defined(__gnu_lineux__)
#	error "Linux is not supported!"
#	define MYL_PLATFORM_LINUX
#elif defined(__ANDROID__)
#	error "Android is not supported!"
#	define MYL_PLATFORM_ANDROID
#elif defined(__unix__)
#	error "Unix is not supported!"
#	define MYL_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
#	error "Posix is not supported!"
#	define MYL_PLATFORM_POSIX
#else
#	error "Unknown platform"
#endif