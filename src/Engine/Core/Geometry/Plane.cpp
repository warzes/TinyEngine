#include "stdafx.h"
#include "Plane.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingFrustum.h"
#include "Core/Math/MathCoreFunc.h"

Plane::Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2) noexcept
{
	// NOTE: Left-handed coordinate system (not right-handed one)
	const auto vector1 = point1 - point0;
	const auto vector2 = point2 - point0;
	normal = glm::normalize(glm::cross(vector1, vector2));
	distance = -glm::dot(normal, point0);
}

void Plane::Normalize() noexcept
{
	const auto length = glm::length(normal);

	if( length >= std::numeric_limits<float>::epsilon() )
	{
		const auto inverseLength = 1.0f / length;
		normal *= inverseLength;
		distance *= inverseLength;
	}
}

Plane Plane::Normalize(const Plane& plane) noexcept
{
	auto result = plane;
	result.Normalize();
	return result;
}

float Plane::Dot(const glm::vec4& vec) const noexcept
{
	return normal.x * vec.x +
		normal.y * vec.y +
		normal.z * vec.z +
		distance * vec.w;
}

float Plane::DotCoordinate(const glm::vec3& vec) const noexcept
{
	return normal.x * vec.x + normal.y * vec.y + normal.z * vec.z + distance;
}

float Plane::DotNormal(const glm::vec3& vec) const noexcept
{
	return glm::dot(normal, vec);
}

float Plane::GetDistanceToPoint(const glm::vec3& point) const noexcept
{
	return DotCoordinate(point);
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
	return frustum.Intersects(*this);
}

PlaneIntersectionType Plane::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto dotProduct = DotCoordinate(sphere.center);
	if( dotProduct > sphere.radius ) return PlaneIntersectionType::Front;
	if( dotProduct < -sphere.radius ) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}

Plane Plane::Transform(const Plane& plane, const glm::mat4& matrix)
{
	const auto transformMatrix = glm::inverse(matrix);
	const auto vector = glm::vec4{ plane.normal, plane.distance };
	const auto transformedVector = ::Transform(vector, transformMatrix);

	Plane result;
	result.normal.x = transformedVector.x;
	result.normal.y = transformedVector.y;
	result.normal.z = transformedVector.z;
	result.distance = transformedVector.w;
	return result;
}

Plane Plane::CreateFromPointNormal(const glm::vec3& point, const glm::vec3& normal)
{
	return Plane(normal, -glm::dot(normal, point));
}
