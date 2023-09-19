#include "stdafx.h"
#include "BoundingAABB.h"
#include "Plane.h"
#include "BoundingSphere.h"
#include "Ray.h"
#include "Core/Math/MathCoreFunc.h"
//-----------------------------------------------------------------------------
void BoundingAABB::AddPoint(const glm::vec3& point)
{
	min = glm::min(point, min);
	max = glm::max(point, max);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Merge(const BoundingAABB& rhs)
{
	AddPoint(rhs.min);
	AddPoint(rhs.max);
}
//-----------------------------------------------------------------------------
void BoundingAABB::Translate(const glm::vec3& v)
{
	min += v;
	max += v;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const glm::vec3& point) const noexcept
{
	if( (point.x < min.x) ||
		(point.y < min.y) ||
		(point.z < min.z) ||
		(point.x > max.x) ||
		(point.y > max.y) ||
		(point.z > max.z) )
	{
		return ContainmentType::Disjoint;
	}
	if( (point.x == min.x) ||
		(point.y == min.y) ||
		(point.z == min.z) ||
		(point.x == max.x) ||
		(point.y == max.y) ||
		(point.z == max.z) )
	{
		return ContainmentType::Intersects;
	}
	return ContainmentType::Contains;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const BoundingAABB& box) const noexcept
{
	if( (min.x > box.max.x || max.x < box.min.x) ||
		(min.y > box.max.y || max.y < box.min.y) ||
		(min.z > box.max.z || max.z < box.min.z) )
	{
		return ContainmentType::Disjoint;
	}
	if( (min.x <= box.min.x && box.max.x <= max.x) &&
		(min.y <= box.min.y && box.max.y <= max.y) &&
		(min.z <= box.min.z && box.max.z <= max.z) )
	{
		return ContainmentType::Contains;
	}
	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingAABB::Contains(const BoundingSphere& sphere) const noexcept
{
	const auto clamped = glm::clamp(sphere.center, min, max);
	const auto distanceSquared = DistanceSquared(sphere.center, clamped);

	if( distanceSquared > sphere.radius * sphere.radius )
	{
		return ContainmentType::Disjoint;
	}
	if( (sphere.radius <= sphere.center.x - min.x) &&
		(sphere.radius <= sphere.center.y - min.y) &&
		(sphere.radius <= sphere.center.z - min.z) &&
		(sphere.radius <= max.x - sphere.center.x) &&
		(sphere.radius <= max.y - sphere.center.y) &&
		(sphere.radius <= max.z - sphere.center.z) )
	{
		return ContainmentType::Contains;
	}
	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------------
bool BoundingAABB::Intersects(const BoundingAABB& box) const noexcept
{
	return 
		(max.x >= box.min.x && min.x <= box.max.x) &&
		(max.y >= box.min.y && min.y <= box.max.y) &&
		(max.z >= box.min.z && min.z <= box.max.z);
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