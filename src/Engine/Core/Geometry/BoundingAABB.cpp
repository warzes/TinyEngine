#include "stdafx.h"
#include "BoundingAABB.h"
#include "Plane.h"
#include "BoundingSphere.h"
#include "Ray.h"
#include "Core/Math/MathLib.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
void BoundingAABB::Insert(const glm::vec3& point) noexcept
{
	min = glm::min(point, min);
	max = glm::max(point, max);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Insert(const BoundingAABB& otherAABB) noexcept
{
	Insert(otherAABB.min);
	Insert(otherAABB.max);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Translate(const glm::vec3& position) noexcept
{
	min += position;
	max += position;
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
ContainmentType BoundingAABB::Contains(const BoundingSphere& sphere) const noexcept
{
	const auto clamped = glm::clamp(sphere.center, min, max);
	const auto distanceSquared = DistanceSquared(sphere.center, clamped);

	if( distanceSquared > sphere.radius * sphere.radius )
		return ContainmentType::Disjoint;

	if( (sphere.radius <= sphere.center.x - min.x) &&
		(sphere.radius <= sphere.center.y - min.y) &&
		(sphere.radius <= sphere.center.z - min.z) &&
		(sphere.radius <= max.x - sphere.center.x) &&
		(sphere.radius <= max.y - sphere.center.y) &&
		(sphere.radius <= max.z - sphere.center.z) )
		return ContainmentType::Contains;

	return ContainmentType::Intersects;
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
bool BoundingAABB::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto clamped = glm::clamp(sphere.center, min, max);
	const auto distanceSquared = DistanceSquared(sphere.center, clamped);
	return distanceSquared <= sphere.radius * sphere.radius;
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