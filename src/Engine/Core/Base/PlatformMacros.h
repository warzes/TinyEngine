#pragma once

#define PLATFORM_LINUX 0
#define PLATFORM_BSD 0
#define PLATFORM_WINDOWS 0
#define PLATFORM_EMSCRIPTEN 0
#define PLATFORM_ANDROID 0

#define PLATFORM_DESKTOP 0

#if defined(linux) || defined(__linux) || defined(__linux__)
#	undef  PLATFORM_LINUX
#	define PLATFORM_LINUX 1
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#	undef  PLATFORM_BSD
#	define PLATFORM_BSD 1
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#	undef  PLATFORM_WINDOWS
#	define PLATFORM_WINDOWS 1
#elif defined(__EMSCRIPTEN__)
#	undef  PLATFORM_EMSCRIPTEN
#	define PLATFORM_EMSCRIPTEN 1
#elif defined(ANDROID) || defined(__ANDROID__)
#	undef  PLATFORM_ANDROID
#	define PLATFORM_ANDROID 1
#endif

#if PLATFORM_LINUX || PLATFORM_BSD || PLATFORM_WINDOWS
#	undef  PLATFORM_DESKTOP
#	define PLATFORM_DESKTOP 1
#endif

#if PLATFORM_EMSCRIPTEN
#	undef USE_SSE
#	define USE_SSE 0
#endif