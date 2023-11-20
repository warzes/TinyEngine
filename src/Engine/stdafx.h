#pragma once

#include "Core/Base/PlatformMacros.h"
#include "EngineBuildSettings.h"

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
#include <set>
#include <queue>

#if PLATFORM_DESKTOP
#	include <glad/gl.h>
#endif

#if PLATFORM_EMSCRIPTEN
//#	define GLFW_INCLUDE_ES2           // GLFW3: Enable OpenGL ES 2.0 (translated to WebGL)
#	define GLFW_INCLUDE_ES3          // GLFW3: Enable OpenGL ES 3.0 (transalted to WebGL2?)
//#	define GL_GLEXT_PROTOTYPES
//#	define EGL_EGLEXT_PROTOTYPES
#	include <emscripten/emscripten.h> // Emscripten functionality for C
#	include <emscripten/html5.h>      // Emscripten HTML5 library
#endif

#include <glfw.h>

#include <stb/stb_image.h>

#include <tiny_obj_loader.h>

#include <glmHeader.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif