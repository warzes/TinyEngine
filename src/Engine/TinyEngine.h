﻿#pragma once

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
#include <vector>

#include <glad/gl.h>
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
// IO
//-----------------------------------------------------------------------------
#include "Core/IO/FileSystem.h"
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
#include "Core/Geometry/AABB.h"
#include "Core/Geometry/GeometryShapes.h"
#include "Core/Geometry/Collisions.h"
//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------
#include "CoreUtilities.h"
#include "Core/Utilities/StringUtilities.h"

//=============================================================================
// Platform
//=============================================================================

#include "Platform/Monitor.h"
#include "Platform/Window.h"
#include "Platform/Input.h"

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