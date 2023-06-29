#include "stdafx.h"
#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "Ray.h"
#include "Plane.h"
#include "Core/Math/MathCoreFunc.h"

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

PlaneIntersectionType BoundingSphere::Intersects(const Plane& plane) const noexcept
{
	return plane.Intersects(*this);
}

std::optional<float> BoundingSphere::Intersects(const Ray& ray) const noexcept
{
	return ray.Intersects(*this);
}

BoundingSphere BoundingSphere::CreateFromPoints(const glm::vec3* points, std::size_t pointCount) noexcept
{
	auto accessor = [&](std::size_t i) -> glm::vec3 { return points[i]; };
	return CreateFromPoints(std::move(accessor), pointCount);
}

BoundingSphere BoundingSphere::CreateFromPoints(std::function<glm::vec3(std::size_t)> points, std::size_t pointCount) noexcept
{
	assert(points != nullptr);
	assert(pointCount > 0);

	// NOTE: Compute bounding sphere using Jack Ritter's algorithm.
	std::size_t maxX = 0;
	std::size_t maxY = 0;
	std::size_t maxZ = 0;
	std::size_t minX = 0;
	std::size_t minY = 0;
	std::size_t minZ = 0;

	for( std::size_t i = 0; i < pointCount; i++ )
	{
		const auto& p = points(i);
		if( p.x < points(minX).x )
		{
			minX = i;
		}
		if( p.x > points(maxX).x )
		{
			maxX = i;
		}
		if( p.y < points(minY).y )
		{
			minY = i;
		}
		if( p.y > points(maxY).y )
		{
			maxY = i;
		}
		if( p.z < points(minZ).z )
		{
			minZ = i;
		}
		if( p.z > points(maxZ).z )
		{
			maxZ = i;
		}
	}

	const auto distX = DistanceSquared(points(maxX), points(minX));
	const auto distY = DistanceSquared(points(maxY), points(minY));
	const auto distZ = DistanceSquared(points(maxZ), points(minZ));

	std::size_t max = maxX;
	std::size_t min = minX;
	if( distY > distX && distY > distZ )
	{
		max = maxY;
		min = minY;
	}
	else if( distZ > distX && distZ > distY )
	{
		max = maxZ;
		min = minZ;
	}

	auto center = (points(max) + points(min)) * 0.5f;
	auto radius = glm::distance(points(max), center);
	auto radiusSq = radius * radius;

	// NOTE: Compute strict bounding sphere.
	for( std::size_t i = 0; i < pointCount; i++ )
	{
		const auto p = points(i);
		const auto diff = p - center;
		const auto distanceSq = LengthSquared(diff);
		if( distanceSq > radiusSq )
		{
			const auto distance = std::sqrt(distanceSq);
			const auto direction = diff / distance;
			const auto g = center - radius * direction;
			center = (g + p) * 0.5f;
			radius = glm::distance(p, center);
			radiusSq = radius * radius;
		}
	}

	BoundingSphere sphere;
	sphere.center = center;
	sphere.radius = radius;
	return sphere;
}
