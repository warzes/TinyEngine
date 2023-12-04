#include "stdafx.h"
#include "BoundingFrustum.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"
#include "Ray.h"
//-----------------------------------------------------------------------------
Frustum::Frustum() noexcept
{
	Set(glm::mat4(1.0f));
}
//-----------------------------------------------------------------------------
Frustum::Frustum(const glm::mat4& transform) noexcept
{
	Set(transform);
}
//-----------------------------------------------------------------------------
Frustum::Frustum(const glm::mat4& projection, const glm::mat4& view) noexcept
{
	glm::mat4 m = projection * view;
	Set(m);
}
//-----------------------------------------------------------------------------
void Frustum::Set(const glm::mat4& projection, const glm::mat4& view) noexcept
{
	glm::mat4 m = projection * view;
	Set(m);
}
//-----------------------------------------------------------------------------
void Frustum::Set(const glm::mat4& transform) noexcept
{
	auto& m = transform;
	m_planes[PLANE_LEFT] = Plane({ m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0] }, m[3][3] + m[3][0]);
	m_planes[PLANE_RIGHT] = Plane({ m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0] }, m[3][3] - m[3][0]);
	m_planes[PLANE_DOWN] = Plane({ m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1] }, m[3][3] + m[3][1]);
	m_planes[PLANE_UP] = Plane({ m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1] }, m[3][3] - m[3][1]);
	m_planes[PLANE_NEAR] = Plane({ m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2] }, m[3][3] + m[3][2]);
	m_planes[PLANE_FAR] = Plane({ m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2] }, m[3][3] - m[3][2]);

	for (int i = 0; i < 6; i++)
		m_planes[i].Normalize();

	calculateVertices(transform);
}
//-----------------------------------------------------------------------------
void Frustum::SetOrtho(float scale, float aspectRatio, float n, float f, const glm::mat4& viewMatrix) noexcept
{
	glm::mat4 m = glm::ortho(-scale * aspectRatio, scale * aspectRatio, -scale, scale, n, f);
	m = m * viewMatrix;
	Set(m);
}
//-----------------------------------------------------------------------------
void Frustum::Set(float fov, float aspectRatio, float n, float f, const glm::mat4& viewMatrix) noexcept
{
	float tangent = tan(fov * 0.5f);
	float height = n * tangent;
	float width = height * aspectRatio;

	glm::mat4 m = glm::frustum(-width, width, -height, height, n, f);
	m = m * viewMatrix;
	Set(m);
}
//-----------------------------------------------------------------------------
void Frustum::Transform(const glm::mat4& transform) noexcept
{
	for (int i = 0; i < 6; i++)
		m_planes[i].Transform(transform);

	for (int i = 0; i < 6; i++)
		m_planes[i].Normalize();

	calculateVertices(transform);
}
//-----------------------------------------------------------------------------
bool Frustum::IsInside(const glm::vec3& point) const noexcept
{
	for (int i = 0; i < 6; i++)
	{
		if (m_planes[i].Distance(point) < 0.0f)
			return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool Frustum::IsInside(const BoundingSphere& sphere) const noexcept
{
	for (int i = 0; i < 6; i++)
	{
		if (m_planes[i].Distance(sphere.center) < -sphere.radius)
			return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool Frustum::IsInside(const BoundingAABB& box) const noexcept
{
	for (int i = 0; i < 6; i++)
	{
		glm::vec3 p = box.min, n = box.max;
		glm::vec3 N = m_planes[i].normal;
		if (N.x >= 0)
		{
			p.x = box.max.x;
			n.x = box.min.x;
		}
		if (N.y >= 0)
		{
			p.y = box.max.y;
			n.y = box.min.y;
		}
		if (N.z >= 0)
		{
			p.z = box.max.z;
			n.z = box.min.z;
		}

		if (m_planes[i].Distance(p) < 0)
			return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool Frustum::IsInside(const Plane& plane) const noexcept
{
	for (int i = 0; i < 6; i++)
	{
		if (m_planes[i].Distance(plane.normal) < 0.0f)
			return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool Frustum::IsInside(const Ray& ray) const noexcept
{
	for (int i = 0; i < 6; i++)
	{
		if (m_planes[i].Distance(ray.position) < 0.0f)
			return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
const Plane& Frustum::GetPlane(FrustumPlane plane) const noexcept
{
	return m_planes[plane];
}
//-----------------------------------------------------------------------------
glm::vec3* Frustum::GetVerticies() noexcept
{
	return m_verticies;
}
//-----------------------------------------------------------------------------
void Frustum::calculateVertices(const glm::mat4& transform) noexcept
{
	static const bool zerotoOne = false;
	static const bool leftHand = true;

	glm::mat4 transformInv = glm::inverse(transform);

	m_verticies[0] = glm::vec4(-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
	m_verticies[1] = glm::vec4(1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
	m_verticies[2] = glm::vec4(1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
	m_verticies[3] = glm::vec4(-1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);

	m_verticies[4] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	m_verticies[5] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
	m_verticies[6] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_verticies[7] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec4 temp;
	for (int i = 0; i < 8; i++)
	{
		temp = transformInv * glm::vec4(m_verticies[i], 1.0f);
		m_verticies[i] = temp / temp.w;
	}
}
//-----------------------------------------------------------------------------
namespace PlaneIndex
{
	constexpr int Near = 0;
	constexpr int Far = 1;
	constexpr int Left = 2;
	constexpr int Right = 3;
	constexpr int Top = 4;
	constexpr int Bottom = 5;
} // namespace PlaneIndex
//-----------------------------------------------------------------------------
namespace CornerIndex
{
	constexpr int NearLeftTop = 0;
	constexpr int NearRightTop = 1;
	constexpr int NearRightBottom = 2;
	constexpr int NearLeftBottom = 3;
	constexpr int FarLeftTop = 4;
	constexpr int FarRightTop = 5;
	constexpr int FarRightBottom = 6;
	constexpr int FarLeftBottom = 7;
} // namespace CornerIndex
//-----------------------------------------------------------------------------
#define CREATE_PLANES_BEFORE_CORNERS 1
#define BOUNDING_FRUSTUM_FAST_CONTAINS 1
//-----------------------------------------------------------------------------
namespace
{
#if CREATE_PLANES_BEFORE_CORNERS

	void makePlane(Plane& plane, float x, float y, float z, float d)
	{
		plane.normal.x = x;
		plane.normal.y = y;
		plane.normal.z = z;
		plane.distance = d;
	}

	[[nodiscard]] glm::vec3 computeIntersectionPoint(const Plane& a, const Plane& b, const Plane& c)
	{
		const auto cross = glm::cross(b.normal, c.normal);
		const auto denom = -1.0f * glm::dot(a.normal, cross);
		const auto v1 = a.distance * glm::cross(b.normal, c.normal);
		const auto v2 = b.distance * glm::cross(c.normal, a.normal);
		const auto v3 = c.distance * glm::cross(a.normal, b.normal);

		glm::vec3 result = (v1 + v2 + v3) / denom;
		return result;
	}

#endif

	constexpr auto NLT = CornerIndex::NearLeftTop;
	constexpr auto NRT = CornerIndex::NearRightTop;
	constexpr auto NRB = CornerIndex::NearRightBottom;
	constexpr auto NLB = CornerIndex::NearLeftBottom;
	constexpr auto FLT = CornerIndex::FarLeftTop;
	constexpr auto FRT = CornerIndex::FarRightTop;
	constexpr auto FRB = CornerIndex::FarRightBottom;
	constexpr auto FLB = CornerIndex::FarLeftBottom;

} // namespace
//-----------------------------------------------------------------------------
OldBoundingFrustum::OldBoundingFrustum(const glm::mat4& matrix)
{
	SetMatrix(matrix);
}
//-----------------------------------------------------------------------------
const glm::mat4& OldBoundingFrustum::GetMatrix() const noexcept
{
	return m_matrix;
}
//-----------------------------------------------------------------------------
void OldBoundingFrustum::SetMatrix(const glm::mat4& matrix) noexcept
{
	m_matrix = matrix;

#if CREATE_PLANES_BEFORE_CORNERS
	createPlanes();
	createCorners();
#else
	createCorners();
	createPlanes();
#endif
}
//-----------------------------------------------------------------------------
const Plane& OldBoundingFrustum::GetNear() const noexcept
{
	return m_planes[PlaneIndex::Near];
}
//-----------------------------------------------------------------------------
const Plane& OldBoundingFrustum::GetFar() const noexcept
{
	return m_planes[PlaneIndex::Far];
}
//-----------------------------------------------------------------------------
const Plane& OldBoundingFrustum::GetLeft() const noexcept
{
	return m_planes[PlaneIndex::Left];
}
//-----------------------------------------------------------------------------
const Plane& OldBoundingFrustum::GetRight() const noexcept
{
	return m_planes[PlaneIndex::Right];
}
//-----------------------------------------------------------------------------
const Plane& OldBoundingFrustum::GetTop() const noexcept
{
	return m_planes[PlaneIndex::Top];
}
//-----------------------------------------------------------------------------
const Plane& OldBoundingFrustum::GetBottom() const noexcept
{
	return m_planes[PlaneIndex::Bottom];
}
//-----------------------------------------------------------------------------
const std::array<glm::vec3, OldBoundingFrustum::CornerCount>& OldBoundingFrustum::GetCorners() const noexcept
{
	return m_corners;
}
//-----------------------------------------------------------------------------
ContainmentType OldBoundingFrustum::Contains(const glm::vec3& point) const noexcept
{
#if BOUNDING_FRUSTUM_FAST_CONTAINS
	// NOTE: fast mode
	for( auto& plane : m_planes )
	{
		const auto distance = plane.Distance(point);
		if( distance > 0.0f ) return ContainmentType::Disjoint;
	}
	return ContainmentType::Contains;
#else
	bool intersects = false;
	for( auto& plane : m_planes )
	{
		const auto distance = plane.Distance(point);
		if( distance > 0.0f ) return ContainmentType::Disjoint;
		if( distance == 0.0f ) intersects = true;
	}
	if( intersects ) return ContainmentType::Intersects;
	return ContainmentType::Contains;
#endif
}
//-----------------------------------------------------------------------------
ContainmentType OldBoundingFrustum::Contains(const BoundingAABB& box) const noexcept
{
	bool intersects = false;
	for( auto& plane : m_planes )
	{
		const auto planeIntersectionType = plane.Intersects(box);
		if( planeIntersectionType == PlaneIntersectionType::Front )
		{
			return ContainmentType::Disjoint;
		}
		if( planeIntersectionType == PlaneIntersectionType::Intersecting )
		{
			intersects = true;
		}
	}
	if( intersects )
	{
		return ContainmentType::Intersects;
	}
	return ContainmentType::Contains;
}
//-----------------------------------------------------------------------------
ContainmentType OldBoundingFrustum::Contains(const OldBoundingFrustum& frustum) const noexcept
{
	//bool intersects = false;
	//for( auto& plane : m_planes )
	//{
	//	const auto planeIntersectionType = plane.Intersects(frustum);
	//	if( planeIntersectionType == PlaneIntersectionType::Front )
	//	{
	//		return ContainmentType::Disjoint;
	//	}
	//	if( planeIntersectionType == PlaneIntersectionType::Intersecting )
	//	{
	//		intersects = true;
	//	}
	//}
	//if( intersects )
	//{
	//	return ContainmentType::Intersects;
	//}
	return ContainmentType::Contains;
}
//-----------------------------------------------------------------------------
ContainmentType OldBoundingFrustum::Contains(const BoundingSphere& sphere) const noexcept
{
	bool intersects = false;
	for( auto& plane : m_planes )
	{
		const auto planeIntersectionType = plane.Intersects(sphere);
		if( planeIntersectionType == PlaneIntersectionType::Front )
		{
			return ContainmentType::Disjoint;
		}
		if( planeIntersectionType == PlaneIntersectionType::Intersecting )
		{
			intersects = true;
		}
	}
	if( intersects )
	{
		return ContainmentType::Intersects;
	}
	return ContainmentType::Contains;
}
//-----------------------------------------------------------------------------
bool OldBoundingFrustum::Intersects(const BoundingAABB& box) const noexcept
{
	for( auto& plane : m_planes )
	{
		if( plane.Intersects(box) == PlaneIntersectionType::Front )
		{
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool OldBoundingFrustum::Intersects(const OldBoundingFrustum& frustum) const noexcept
{
	//for( auto& plane : m_planes )
	//{
	//	const auto planeIntersectionType = plane.Intersects(frustum);
	//	if( planeIntersectionType == PlaneIntersectionType::Front )
	//	{
	//		return false;
	//	}
	//}
	//return true;
	return false;
}
//-----------------------------------------------------------------------------
bool OldBoundingFrustum::Intersects(const BoundingSphere& sphere) const noexcept
{
	for( auto& plane : m_planes )
	{
		const auto planeIntersectionType = plane.Intersects(sphere);
		if( planeIntersectionType == PlaneIntersectionType::Front )
		{
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
PlaneIntersectionType OldBoundingFrustum::Intersects(const Plane& plane) const noexcept
{
	auto result = plane.Intersects(m_corners.front());
	for( std::size_t i = 1; i < m_corners.size(); ++i )
	{
		if( result != plane.Intersects(m_corners[i]) )
		{
			result = PlaneIntersectionType::Intersecting;
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
std::optional<float> OldBoundingFrustum::Intersects(const Ray & ray) const noexcept
{
	std::array<std::optional<float>, PlaneCount> distances;
	for( int i = 0; i < PlaneCount; ++i )
	{
		auto& plane = m_planes[i];
		const auto d = ray.Intersects(plane);
		const auto distanceToRayPosition = plane.Distance(ray.position);
		if( (distanceToRayPosition < 0.0f) && !d )
		{
			return std::nullopt;
		}
		distances[i] = d;
		assert(!distances[i] || *distances[i] >= 0.0f);
	}

	auto d = distances.front();
	assert(!d || *d >= 0.0f);
	for( int i = 1; i < PlaneCount; ++i )
	{
		if( !d )
		{
			d = distances[i];
			assert(!d || *d >= 0.0f);
			continue;
		}
		if( distances[i] )
		{
			assert(d);
			d = std::min(*distances[i], *d);
			assert(d && *d >= 0.0f);
		}
	}
	assert(!d || *d >= 0.0f);
	return d;
}
//-----------------------------------------------------------------------------
void OldBoundingFrustum::createPlanes()
{
#if CREATE_PLANES_BEFORE_CORNERS
	makePlane(
		m_planes[PlaneIndex::Near],
		-m_matrix[0][2],
		-m_matrix[1][2],
		-m_matrix[2][2],
		-m_matrix[3][2]);

	makePlane(
		m_planes[PlaneIndex::Far],
		-m_matrix[0][3] + m_matrix[0][2],
		-m_matrix[1][3] + m_matrix[1][2],
		-m_matrix[2][3] + m_matrix[2][2],
		-m_matrix[3][3] + m_matrix[3][2]);

	makePlane(
		m_planes[PlaneIndex::Left],
		-m_matrix[0][3] - m_matrix[0][0],
		-m_matrix[1][3] - m_matrix[1][0],
		-m_matrix[2][3] - m_matrix[2][0],
		-m_matrix[3][3] - m_matrix[3][0]);

	makePlane(
		m_planes[PlaneIndex::Right],
		-m_matrix[0][3] + m_matrix[0][0],
		-m_matrix[1][3] + m_matrix[1][0],
		-m_matrix[2][3] + m_matrix[2][0],
		-m_matrix[3][3] + m_matrix[3][0]);

	makePlane(
		m_planes[PlaneIndex::Top],
		-m_matrix[0][3] + m_matrix[0][1],
		-m_matrix[1][3] + m_matrix[1][1],
		-m_matrix[2][3] + m_matrix[2][1],
		-m_matrix[3][3] + m_matrix[3][1]);

	makePlane(
		m_planes[PlaneIndex::Bottom],
		-m_matrix[0][3] - m_matrix[0][1],
		-m_matrix[1][3] - m_matrix[1][1],
		-m_matrix[2][3] - m_matrix[2][1],
		-m_matrix[3][3] - m_matrix[3][1]);

	for( auto& plane : m_planes )
	{
		plane.Normalize();
	}
#else
	// NOTE: Left-handed coordinate system
	//
	//   NLT
	//    |\.
	//    |  \.
	//    |  =======> Normal
	//    |      \.
	//    |------- \.
	//   NLB        NRB
	//
	// Normal == Normalize(Cross(NLT - NLB, NRB - NLT))

	m_planes[PlaneIndex::Near] = Plane{ m_corners[NLB], m_corners[NLT], m_corners[NRB] };
	m_planes[PlaneIndex::Far] = Plane{ m_corners[FRB], m_corners[FRT], m_corners[FLB] };
	m_planes[PlaneIndex::Left] = Plane{ m_corners[FLB], m_corners[FLT], m_corners[NLB] };
	m_planes[PlaneIndex::Right] = Plane{ m_corners[NRB], m_corners[NRT], m_corners[FRB] };
	m_planes[PlaneIndex::Top] = Plane{ m_corners[NLT], m_corners[FLT], m_corners[NRT] };
	m_planes[PlaneIndex::Bottom] = Plane{ m_corners[FLB], m_corners[NLB], m_corners[FRB] };
#endif
}
//-----------------------------------------------------------------------------
void OldBoundingFrustum::createCorners()
{
#if CREATE_PLANES_BEFORE_CORNERS
	constexpr auto N = PlaneIndex::Near;
	constexpr auto F = PlaneIndex::Far;
	constexpr auto L = PlaneIndex::Left;
	constexpr auto R = PlaneIndex::Right;
	constexpr auto T = PlaneIndex::Top;
	constexpr auto B = PlaneIndex::Bottom;

	m_corners[NLT] = computeIntersectionPoint(m_planes[N], m_planes[L], m_planes[T]);
	m_corners[NRT] = computeIntersectionPoint(m_planes[N], m_planes[R], m_planes[T]);
	m_corners[NRB] = computeIntersectionPoint(m_planes[N], m_planes[R], m_planes[B]);
	m_corners[NLB] = computeIntersectionPoint(m_planes[N], m_planes[L], m_planes[B]);
	m_corners[FLT] = computeIntersectionPoint(m_planes[F], m_planes[L], m_planes[T]);
	m_corners[FRT] = computeIntersectionPoint(m_planes[F], m_planes[R], m_planes[T]);
	m_corners[FRB] = computeIntersectionPoint(m_planes[F], m_planes[R], m_planes[B]);
	m_corners[FLB] = computeIntersectionPoint(m_planes[F], m_planes[L], m_planes[B]);
#else
	constexpr auto xMin = -1.0f;
	constexpr auto xMax = +1.0f;
	constexpr auto yMin = -1.0f;
	constexpr auto yMax = +1.0f;
	constexpr auto zMin = -1.0f;
	constexpr auto zMax = +1.0f;

	// NOTE: Near clip
	m_corners[NLT] = glm::vec3{ xMin, yMax, zMin };
	m_corners[NRT] = glm::vec3{ xMax, yMax, zMin };
	m_corners[NRB] = glm::vec3{ xMax, yMin, zMin };
	m_corners[NLB] = glm::vec3{ xMin, yMin, zMin };

	// NOTE: Far clip
	m_corners[FLT] = glm::vec3{ xMin, yMax, zMax };
	m_corners[FRT] = glm::vec3{ xMax, yMax, zMax };
	m_corners[FRB] = glm::vec3{ xMax, yMin, zMax };
	m_corners[FLB] = glm::vec3{ xMin, yMin, zMax };

	auto inverseMatrix = glm::invert(m_matrix);
	for( auto& corner : m_corners )
	{
		corner = glm::transform(corner, inverseMatrix);
	}
#endif
}
//-----------------------------------------------------------------------------