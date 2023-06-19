#include "stdafx.h"
#include "Plane.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"

Plane::Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2) noexcept
{
	// NOTE: Left-handed coordinate system (not right-handed one)
	const auto vector1 = point1 - point0;
	const auto vector2 = point2 - point0;
	normal = glm::normalize(glm::cross(vector1, vector2));
	distance = -glm::dot(normal, point0);
}

float Plane::DotCoordinate(const glm::vec3& vec) const noexcept
{
	return normal.x * vec.x + normal.y * vec.y + normal.z * vec.z + distance;
}

PlaneIntersectionType Plane::Intersects(const glm::vec3& point) const noexcept
{
	const auto dotProduct = DotCoordinate(point);
	if( dotProduct > 0.0f ) return PlaneIntersectionType::Front;
	if( dotProduct < 0.0f ) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType Plane::Intersects(const BoundingBox& box) const noexcept
{
	glm::vec3 positiveVertex{ box.min.x, box.min.y, box.min.z };
	glm::vec3 negativeVertex{ box.max.x, box.max.y, box.max.z };

	if( normal.x >= 0.0f )
	{
		positiveVertex.x = box.max.x;
		negativeVertex.x = box.min.x;
	}
	if( normal.y >= 0.0f )
	{
		positiveVertex.y = box.max.y;
		negativeVertex.y = box.min.y;
	}
	if( normal.z >= 0.0f )
	{
		positiveVertex.z = box.max.z;
		negativeVertex.z = box.min.z;
	}

	if( DotCoordinate(negativeVertex) > 0.0f ) return PlaneIntersectionType::Front;
	if( DotCoordinate(positiveVertex) < 0.0f ) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType Plane::Intersects(const BoundingFrustum& frustum) const noexcept
{
	return PlaneIntersectionType();
}

PlaneIntersectionType Plane::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto dotProduct = DotCoordinate(sphere.center);
	if( dotProduct > sphere.radius ) return PlaneIntersectionType::Front;
	if( dotProduct < -sphere.radius ) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}