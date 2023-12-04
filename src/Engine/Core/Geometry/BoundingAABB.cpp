#include "stdafx.h"
#include "BoundingAABB.h"
#include "Plane.h"
#include "BoundingSphere.h"
#include "Ray.h"
#include "Core/Math/MathLib.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
BoundingAABB::BoundingAABB(const glm::vec3* points, size_t numPoints) noexcept
{
	Set(points, numPoints);
}
//-----------------------------------------------------------------------------
BoundingAABB::BoundingAABB(const std::vector<glm::vec3> points) noexcept
{
	Set(points.data(), points.size());
}
//-----------------------------------------------------------------------------
void BoundingAABB::Set(const glm::vec3& _min, const glm::vec3& _max) noexcept
{
	min = _min;
	max = _max;
}
//-----------------------------------------------------------------------------
void BoundingAABB::Set(float fmin, float fmax) noexcept
{
	min = glm::vec3(fmin);
	max = glm::vec3(fmax);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Set(const glm::vec3* points, size_t numPoints) noexcept
{
	min = glm::vec3(FLT_MAX);
	max = glm::vec3(-FLT_MAX);

	for (size_t i = 0; i < numPoints; i++)
	{
		min = glm::min(min, points[i]);
		max = glm::max(max, points[i]);
	}
}
//-----------------------------------------------------------------------------
void BoundingAABB::Set(const glm::vec3* points, size_t numPoints, const glm::mat4& transform) noexcept
{
	min = glm::vec3(FLT_MAX);
	max = glm::vec3(-FLT_MAX);

	for (size_t i = 0; i < numPoints; i++)
	{
		const glm::vec3 transformed = transform * glm::vec4(points[i], 1.0f);

		min = glm::min(min, transformed);
		max = glm::max(max, transformed);
	}
}
//-----------------------------------------------------------------------------
void BoundingAABB::SetFromTransformedAABB(const BoundingAABB& aabb, const glm::mat4& transform) noexcept
{
	const glm::vec3 minTransformed = transform * glm::vec4(aabb.min, 1.0f);
	const glm::vec3 maxTransformed = transform * glm::vec4(aabb.max, 1.0f);

	min = glm::min(minTransformed, maxTransformed);
	max = glm::max(minTransformed, maxTransformed);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const BoundingAABB& other) noexcept
{
	min = glm::min(min, other.min);
	max = glm::max(max, other.max);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const glm::vec3& point) noexcept
{
	min = glm::min(point, min);
	max = glm::max(point, max);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const BoundingAABB& other, const glm::mat4& transform) noexcept
{
	Merge(other.Transformed(transform));
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const glm::vec3& point, const glm::mat4& transform) noexcept
{
	glm::vec3 transformed = transform * glm::vec4(point, 1.0f);
	Merge(transformed);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const BoundingAABB& other, const glm::mat3& transform) noexcept
{
	Merge(other.Transformed(glm::mat4(transform)));
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const glm::vec3& point, const glm::mat3& transform) noexcept
{
	glm::vec3 transformed = transform * glm::vec4(point, 1.0f);
	Merge(transformed);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Translate(const glm::vec3& position) noexcept
{
	min += position;
	max += position;
}
//-----------------------------------------------------------------------------
void BoundingAABB::Scale(const glm::vec3& scale) noexcept
{
	min *= scale;
	max *= scale;
}
//-----------------------------------------------------------------------------
void BoundingAABB::Rotate(const glm::mat3& rotation) noexcept
{
	glm::vec3 center = GetCenter();
	glm::vec3 extents = GetSize();

	glm::vec3 rotatedExtents = glm::vec3(rotation * glm::vec4(extents, 1.0f));

	min = center - rotatedExtents;
	max = center + rotatedExtents;
}
//-----------------------------------------------------------------------------
void BoundingAABB::Transform(const glm::mat4& transform) noexcept
{
	glm::vec3 newCenter = transform * glm::vec4(GetCenter(), 1.0f);
	glm::vec3 oldEdge = GetHalfSize();
	glm::vec3 newEdge = glm::vec3(
		glm::abs(transform[0][0]) * oldEdge.x + glm::abs(transform[1][0]) * oldEdge.y + glm::abs(transform[2][0]) * oldEdge.z,
		glm::abs(transform[0][1]) * oldEdge.x + glm::abs(transform[1][1]) * oldEdge.y + glm::abs(transform[2][1]) * oldEdge.z,
		glm::abs(transform[0][2]) * oldEdge.x + glm::abs(transform[1][2]) * oldEdge.y + glm::abs(transform[2][2]) * oldEdge.z);

	min = newCenter - newEdge;
	max = newCenter + newEdge;
}
//-----------------------------------------------------------------------------
void BoundingAABB::Transform(float /*scale*/, const glm::vec3& /*rotation*/, const glm::vec3& /*translation*/) noexcept
{
	glm::mat4 transform = glm::mat4(1.0f);
	// TODO: провести все операции трансформации над матрицей
	assert(0);
	Transform(transform);
}
//-----------------------------------------------------------------------------
BoundingAABB BoundingAABB::Transformed(const glm::mat4& transform) const noexcept
{
	BoundingAABB aabb(*this);
	aabb.Transform(transform);
	return aabb;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const glm::vec3& point) const noexcept
{
	if( (point.x < min.x) || (point.y < min.y) || (point.z < min.z) ||
		(point.x > max.x) || (point.y > max.y) || (point.z > max.z) )
		return ContainmentType::Disjoint;
	if( (point.x == min.x) || (point.y == min.y) || (point.z == min.z) ||
		(point.x == max.x) || (point.y == max.y) || (point.z == max.z) )
		return ContainmentType::Intersects;

	return ContainmentType::Contains;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const Triangle& /*tri*/) const noexcept
{
	assert(0); // TODO: нереализовано
	return ContainmentType();
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const BoundingSphere& sphere) const noexcept
{
	const auto clamped = glm::clamp(sphere.center, min, max);
	const auto distanceSquared = DistanceSquared(sphere.center, clamped);

	if (distanceSquared > sphere.radius * sphere.radius)
		return ContainmentType::Disjoint;

	if ((sphere.radius <= sphere.center.x - min.x) &&
		(sphere.radius <= sphere.center.y - min.y) &&
		(sphere.radius <= sphere.center.z - min.z) &&
		(sphere.radius <= max.x - sphere.center.x) &&
		(sphere.radius <= max.y - sphere.center.y) &&
		(sphere.radius <= max.z - sphere.center.z))
		return ContainmentType::Contains;

	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const BoundingAABB& aabb) const noexcept
{
	if( (min.x > aabb.max.x || max.x < aabb.min.x) ||
		(min.y > aabb.max.y || max.y < aabb.min.y) ||
		(min.z > aabb.max.z || max.z < aabb.min.z) )
		return ContainmentType::Disjoint;
	if( (min.x <= aabb.min.x && aabb.max.x <= max.x) &&
		(min.y <= aabb.min.y && aabb.max.y <= max.y) &&
		(min.z <= aabb.min.z && aabb.max.z <= max.z) )
		return ContainmentType::Contains;

	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const BoundingOrientedBox& /*box*/) const noexcept
{
	assert(0); // TODO: нереализовано
	return ContainmentType();
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const BoundingFrustum& /*fr*/) const noexcept
{
	assert(0); // TODO: нереализовано
	return ContainmentType();
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const glm::vec3& point) const noexcept
{
	if (point.x >= min.x && point.x <= max.x &&
		point.y >= min.y && point.y <= max.y &&
		point.z >= min.z && point.z <= max.z)
	{
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const glm::vec3& point0, const glm::vec3& point1) const noexcept
{
	glm::vec3 c = GetCenter();
	glm::vec3 e = GetSize();
	glm::vec3 m = (point0 + point1) * 0.5f;
	glm::vec3 d = point1 - m;
	m = m - c;

	float adx = fabsf(d.x);
	if (fabsf(m.x) > e.x + adx)
		return false;
	float ady = fabsf(d.y);
	if (fabsf(m.y) > e.y + ady)
		return false;
	float adz = fabsf(d.z);
	if (fabsf(m.z) > e.z + adz)
		return false;

	const float kEps = 1e-6f;
	adx += kEps;
	ady += kEps;
	adz += kEps;

	if (fabsf(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady)
		return false;
	if (fabsf(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx)
		return false;
	if (fabsf(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx)
		return false;
	return true;
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const Triangle& /*tri*/) const noexcept
{
	assert(0); // TODO: нереализовано
	return false;
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const BoundingSphere& sphere) const noexcept
{
	const glm::vec3 clamped = glm::clamp(sphere.center, min, max);
	const float distanceSquared = DistanceSquared(sphere.center, clamped);
	return distanceSquared <= sphere.radius * sphere.radius;
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const BoundingAABB& aabb) const noexcept
{
	return 
		(max.x >= aabb.min.x && min.x <= aabb.max.x) &&
		(max.y >= aabb.min.y && min.y <= aabb.max.y) &&
		(max.z >= aabb.min.z && min.z <= aabb.max.z);
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const BoundingOrientedBox& /*box*/) const noexcept
{
	assert(0); // TODO: нереализовано
	return false;
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const BoundingFrustum& /*fr*/) const noexcept
{
	assert(0); // TODO: нереализовано
	return false;
}
//-----------------------------------------------------------------------------
PlaneIntersectionType BoundingAABB::Intersects(const Plane& plane) const noexcept
{
	return plane.Intersects(*this);
}
//-----------------------------------------------------------------------------
std::optional<float> BoundingAABB::Intersects(const Ray& ray) const noexcept
{
	return ray.Intersects(*this);
}
//-----------------------------------------------------------------------------
std::optional<CollisionHit> BoundingAABB::Hit(const glm::vec3& point) const noexcept
{
	assert(0); // TODO: нереализовано
	return std::optional<CollisionHit>();
}
//-----------------------------------------------------------------------------
std::optional<CollisionHit> BoundingAABB::Hit(const Triangle& tri) const noexcept
{
	assert(0); // TODO: нереализовано
	return std::optional<CollisionHit>();
}
//-----------------------------------------------------------------------------
std::optional<CollisionHit> BoundingAABB::Hit(const BoundingSphere& sphere) const noexcept
{
	assert(0); // TODO: нереализовано
	return std::optional<CollisionHit>();
}
//-----------------------------------------------------------------------------
std::optional<CollisionHit> BoundingAABB::Hit(const BoundingAABB& aabb) const noexcept
{
	assert(0); // TODO: нереализовано
	return std::optional<CollisionHit>();
}
//-----------------------------------------------------------------------------
std::optional<CollisionHit> BoundingAABB::Hit(const BoundingOrientedBox& box) const noexcept
{
	assert(0); // TODO: нереализовано
	return std::optional<CollisionHit>();
}
//-----------------------------------------------------------------------------
std::array<glm::vec3, BoundingAABB::CornerCount> BoundingAABB::GetCorners() const noexcept
{
	return std::array<glm::vec3, BoundingAABB::CornerCount>{{
			{ min.x, max.y, max.z },
			{ max.x, max.y, max.z },
			{ max.x, min.y, max.z },
			{ min.x, min.y, max.z },
			{ min.x, max.y, min.z },
			{ max.x, max.y, min.z },
			{ max.x, min.y, min.z },
			{ min.x, min.y, min.z },
		}};
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(BoundingAABB& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 6)
		return false;

	char* ptr = const_cast<char*>(string);
	out.min.x = (float)strtod(ptr, &ptr);
	out.min.y = (float)strtod(ptr, &ptr);
	out.min.z = (float)strtod(ptr, &ptr);
	out.max.x = (float)strtod(ptr, &ptr);
	out.max.y = (float)strtod(ptr, &ptr);
	out.max.z = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const BoundingAABB& in)
{
	return ToString(in.min) + " " + ToString(in.max);
}
//-----------------------------------------------------------------------------