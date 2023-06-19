#include "stdafx.h"
#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "Plane.h"
#include "MathCoreFunc.h"

ContainmentType BoundingSphere::Contains(const glm::vec3& point) const noexcept
{
	const auto distanceSquared = DistanceSquared(point, center);
	const auto radiusSquared = radius * radius;
	if( distanceSquared > radiusSquared ) return ContainmentType::Disjoint;
	else if( distanceSquared < radiusSquared ) return ContainmentType::Contains;
	return ContainmentType::Intersects;
}

ContainmentType BoundingSphere::Contains(const BoundingBox& box) const noexcept
{
	bool inside = true;
	for( auto& corner : box.GetCorners() )
	{
		if( Contains(corner) == ContainmentType::Disjoint )
		{
			inside = false;
			break;
		}
	}
	if( inside ) return ContainmentType::Contains;
	if( Intersects(box) ) return ContainmentType::Intersects;
	return ContainmentType::Disjoint;
}

ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const noexcept
{
	const auto distance = glm::distance(center, sphere.center);
	if( distance > radius + sphere.radius ) return ContainmentType::Disjoint;
	if( distance + sphere.radius < radius ) return ContainmentType::Contains;
	return ContainmentType::Intersects;
}

bool BoundingSphere::Intersects(const BoundingBox& box) const noexcept
{
	return box.Intersects(*this);
}

bool BoundingSphere::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto distance = glm::distance(center, sphere.center);
	return distance <= radius + sphere.radius;
}

PlaneIntersectionType BoundingSphere::Intersects(const Plane & plane) const noexcept
{
	return plane.Intersects(*this);
}

std::optional<float> BoundingSphere::Intersects(const Ray & ray) const noexcept
{
	return std::optional<float>();
}
