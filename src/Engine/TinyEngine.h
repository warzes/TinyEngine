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