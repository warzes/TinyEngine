#pragma once

#if defined(_WIN32)
#	define FORCEINLINE __forceinline
#else
#	define FORCEINLINE inline
#endif