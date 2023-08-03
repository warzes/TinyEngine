#pragma once
#if defined(__EMSCRIPTEN__)
#	define USE_SSE 0
#else
#	define USE_SSE 1
#endif