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

#include <glad/gl.h>

#include <glfw.h>

#include <stb/stb_image.h>

#include <tiny_obj_loader.h>

#include <glmHeader.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif