#include "stdafx.h"
#include "BoundingBox.h"
#include "Plane.h"
#include "BoundingSphere.h"
#include "MathCoreFunc.h"

ContainmentType BoundingBox::Contains(const glm::vec3& point) const noexcept
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

ContainmentType BoundingBox::Contains(const BoundingBox& box) const noexcept
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

ContainmentType BoundingBox::Contains(const BoundingSphere& sphere) const noexcept
{
	return ContainmentType();
}

bool BoundingBox::Intersects(const BoundingBox& box) const noexcept
{
	return 
		(max.x >= box.min.x && min.x <= box.max.x) &&
		(max.y >= box.min.y && min.y <= box.max.y) &&
		(max.z >= box.min.z && min.z <= box.max.z);
}

bool BoundingBox::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto clamped = std::clamp(sphere.center, min, max);
	const auto distanceSquared = DistanceSquared(sphere.center, clamped);
	return distanceSquared <= sphere.radius * sphere.radius;
}

PlaneIntersectionType BoundingBox::Intersects(const Plane& plane) const noexcept
{
	return plane.Intersects(*this);
}

std::optional<float> BoundingBox::Intersects(const Ray& ray) const noexcept
{
	return std::optional<float>();
}

std::array<glm::vec3, BoundingBox::CornerCount> BoundingBox::GetCorners() const noexcept
{
	return std::array<glm::vec3, BoundingBox::CornerCount>{{
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
