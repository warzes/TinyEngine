#pragma once

//=============================================================================
// Config
//=============================================================================

#include "Core/Base/PlatformMacros.h"
#include "EngineConfiguration.h"

//=============================================================================
// STL and 3rdparty Header
//=============================================================================

#if defined(_MSC_VER)
#	pragma warning(push, 3)
#endif

#include <memory>
#include <algorithm>
#include <sstream>
#include <string>
#include <span>
#include <list>
#include <vector>

#if PLATFORM_DESKTOP
#	include <glad/gl.h>
#endif

#if PLATFORM_EMSCRIPTEN
//#	define GLFW_INCLUDE_ES2           // GLFW3: Enable OpenGL ES 2.0 (translated to WebGL)
#	define GLFW_INCLUDE_ES3           // GLFW3: Enable OpenGL ES 3.0 (transalted to WebGL2?)
//#	define GL_GLEXT_PROTOTYPES
//#	define EGL_EGLEXT_PROTOTYPES
#	include <emscripten/emscripten.h> // Emscripten functionality for C
#	include <emscripten/html5.h>      // Emscripten HTML5 library
#endif

#include <glfw.h>

#include <glmHeader.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4514)
#	pragma warning(disable : 4625)
#	pragma warning(disable : 4626)
#	pragma warning(disable : 4820)
#endif

//=============================================================================
// Core
//=============================================================================

//-----------------------------------------------------------------------------
// Base
//-----------------------------------------------------------------------------
#include "Core/Base/BaseMacros.h"
#include "Core/Base/BaseFunc.h"
//-----------------------------------------------------------------------------
// Containers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// IO
//-----------------------------------------------------------------------------
#include "Core/IO/FileSystem.h"
#include "Core/IO/StringHash.h"
//-----------------------------------------------------------------------------
// Logging
//-----------------------------------------------------------------------------
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
// Math
//-----------------------------------------------------------------------------
#include "Core/Math/MathCoreFunc.h"
#include "Core/Math/Color.h"
#include "Core/Math/Transform.h"
//-----------------------------------------------------------------------------
// Geometry
//-----------------------------------------------------------------------------
#include "Core/Geometry/Plane.h"
#include "Core/Geometry/BoundingAABB.h"
//#include "Core/Geometry/BoundingFrustum.h"
#include "Core/Geometry/BoundingSphere.h"
#include "Core/Geometry/GeometryShapes.h"
#include "Core/Geometry/Triangle.h"

#include "Core/Geometry/Collisions.h"
#include "Core/Geometry/Intersect.h"
//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------
#include "Core/Utilities/CoreUtilities.h"
#include "Core/Utilities/StringUtilities.h"

//=============================================================================
// Platform
//=============================================================================

#include "Platform/Monitor.h"
#include "Platform/WindowSystem.h"
#include "Platform/InputSystem.h"

//=============================================================================
// RenderAPI
//=============================================================================

#include "RenderAPI/RenderResource.h"
#include "RenderAPI/RenderSystem.h"

//=============================================================================
// Graphics
//=============================================================================

#include "Graphics/GraphicsResource.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/DebugDraw.h"

//=============================================================================
// World
//=============================================================================

#include "World/Camera.h"

//=============================================================================
// EngineApp
//=============================================================================

#include "EngineApp/IApp.h"
#include "EngineApp/EngineDevice.h"

//=============================================================================
// End Header
//=============================================================================

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif