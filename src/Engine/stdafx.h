#pragma once

#include "EngineConfiguration.h"

#if defined(_MSC_VER)
#	pragma warning(disable : 4514)
#	pragma warning(disable : 4625)
#	pragma warning(disable : 4626)
#	pragma warning(disable : 4820)
#	pragma warning(disable : 5045)
#	pragma warning(push, 0)
#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <cmath>
#include <cstdio>

#include <memory>
#include <algorithm>
#include <regex>
#include <charconv>
#include <sstream>
#include <fstream>
#include <string>
#include <string_view>
#include <array>
#include <list>
#include <vector>
#include <unordered_map>

#include <glad/gl.h>

#include <glfw.h>

#if defined(_WIN32)
#	include <io.h>
#endif

#include <stb/stb_image.h>

#include <tiny_obj_loader.h>

/*
Left handed
	Y   Z
	|  /
	| /
	|/___X
*/
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_XYZW_ONLY
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtx/matrix_decompose.hpp>

//#include <glm/gtx/normal.hpp>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif