﻿#pragma once

#include "EngineConfiguration.h"
#include "Core/Base/PlatformMacros.h"

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

#include "Core/Base/BaseMacros.h"
#include "Core/Base/BaseFunc.h"

#include "FileSystem.h"
#include "Log.h"

#include "MathCoreFunc.h"
#include "Color.h"
#include "Transform.h"
#include "Plane.h"
#include "AABB.h"
#include "GeometryShapes.h"
#include "Collisions.h"

#include "CoreUtilities.h"

#include "Window.h"
#include "Input.h"

#include "RenderResource.h"
#include "RenderSystem.h"

#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "DebugDraw.h"

#include "Camera.h"

#include "IApp.h"

#include "EngineDevice.h"

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif