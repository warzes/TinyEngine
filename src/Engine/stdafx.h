#pragma once

#include "EngineConfiguration.h"
#include "Core/Base/PlatformMacros.h"

#if defined(_MSC_VER)
#	pragma warning(disable : 4514)
#	pragma warning(disable : 4625)
#	pragma warning(disable : 4626)
#	pragma warning(disable : 4820)
#	pragma warning(disable : 5045)
#	pragma warning(push, 0)
#endif

#include <cassert>
#include <cmath>
#include <cstdio>

#if defined(_MSC_VER) && defined(_DEBUG)
#	define _CRTDBG_MAP_ALLOC
#	ifdef _malloca
#		undef _malloca
#	endif
#	include <crtdbg.h>
#endif

#include <EASTL/algorithm.h>
#include <EASTL/any.h>
#include <EASTL/array.h>
//#include <EASTL/atomic.h>
#include <EASTL/bitset.h>
#include <EASTL/bitvector.h>
#include <EASTL/chrono.h>
#include <EASTL/deque.h>
#include <EASTL/finally.h>
#include <EASTL/fixed_function.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_hash_set.h>
#include <EASTL/fixed_list.h>
#include <EASTL/fixed_map.h>
#include <EASTL/fixed_set.h>
#include <EASTL/fixed_slist.h>
#include <EASTL/fixed_string.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/functional.h>
#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/intrusive_hash_map.h>
#include <EASTL/intrusive_hash_set.h>
#include <EASTL/intrusive_list.h>
#include <EASTL/intrusive_ptr.h>
#include <EASTL/iterator.h>
#include <EASTL/linked_array.h>
#include <EASTL/linked_ptr.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/numeric.h>
#include <EASTL/numeric_limits.h>
#include <EASTL/optional.h>
#include <EASTL/priority_queue.h>
#include <EASTL/set.h>
#include <EASTL/shared_array.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/slist.h>
#include <EASTL/sort.h>
#include <EASTL/span.h>
#include <EASTL/stack.h>
#include <EASTL/string.h>
#include <EASTL/tuple.h>
#include <EASTL/type_traits.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/utility.h>
#include <EASTL/variant.h>
#include <EASTL/vector.h>
#include <EASTL/vector_map.h>
#include <EASTL/vector_multimap.h>
#include <EASTL/vector_multiset.h>
#include <EASTL/vector_set.h>
#include <EASTL/weak_ptr.h>

namespace ea = eastl;

#include <memory>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <string>
#include <string_view>
#include <array>
#include <list>
#include <vector>
#include <unordered_map>
#include <span>

#if PLATFORM_EMSCRIPTEN
#	define GLFW_INCLUDE_ES2           // GLFW3: Enable OpenGL ES 2.0 (translated to WebGL)
//	#define GLFW_INCLUDE_ES3          // GLFW3: Enable OpenGL ES 3.0 (transalted to WebGL2?)
#	include <emscripten/emscripten.h> // Emscripten functionality for C
#	include <emscripten/html5.h>      // Emscripten HTML5 library
#endif

#include <glad/gl.h>
#include <glfw.h>

#include <stb/stb_image.h>

#include <tiny_obj_loader.h>

#include <glmHeader.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif