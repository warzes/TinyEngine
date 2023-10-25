#include "stdafx.h"
#include "Plane.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"
#include "BoundingFrustum.h"
#include "Triangle.h"
#include "Core/Math/MathLib.h"
//-----------------------------------------------------------------------------
const Plane Plane::Up(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
//-----------------------------------------------------------------------------
Plane::Plane(const glm::vec3& inNormal, const glm::vec3& point) noexcept
{
	normal = glm::normalize(inNormal);
	distance = -glm::dot(normal, point);
}
//-----------------------------------------------------------------------------
Plane::Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2) noexcept
{
	// NOTE: Left-handed coordinate system (not right-handed one)
	const auto distVector1 = point1 - point0;
	const auto distVector2 = point2 - point0;
	normal = glm::normalize(glm::cross(distVector1, distVector2));
	distance = -glm::dot(normal, point0);
}
//-----------------------------------------------------------------------------
Plane::Plane(const Triangle& tri) noexcept : Plane(tri.p0, tri.p1, tri.p2)
{
}
//-----------------------------------------------------------------------------
Plane::Plane(const glm::vec4& plane) noexcept
{
	normal = { plane.x, plane.y, plane.z };
	distance = plane.w;
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
Plane Plane::Normalize(const Plane& plane) noexcept
{
	auto result = plane;
	result.Normalize();
	return result;
}
//-----------------------------------------------------------------------------
float Plane::Dot(const glm::vec4& vec) const noexcept
{	
	return normal.x * vec.x + normal.y * vec.y + normal.z * vec.z + distance * vec.w;
}
//-----------------------------------------------------------------------------
float Plane::DotCoordinate(const glm::vec3& vec) const noexcept
{
	return glm::dot(normal, vec) + distance;
}
//-----------------------------------------------------------------------------
float Plane::DotNormal(const glm::vec3& vec) const noexcept
{
	return glm::dot(normal, vec);
}
//-----------------------------------------------------------------------------
glm::vec3 Plane::Project(const glm::vec3& point) const noexcept
{
	return point - normal * (glm::dot(normal, point) + distance);
}
//-----------------------------------------------------------------------------
float Plane::GetDistanceToPoint(const glm::vec3& point) const noexcept
{
	return DotCoordinate(point);
}
//-----------------------------------------------------------------------------
glm::vec3 Plane::Reflect(const glm::vec3& direction) const noexcept
{
	return direction - (2.0f * glm::dot(normal, direction) * normal);
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const glm::vec3& point) const noexcept
{
	const auto dotProduct = DotCoordinate(point);
	if( dotProduct > 0.0f ) return PlaneIntersectionType::Front;
	if( dotProduct < 0.0f ) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const BoundingAABB& box) const noexcept
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
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const BoundingFrustum& frustum) const noexcept
{
	return frustum.Intersects(*this);
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto dotProduct = DotCoordinate(sphere.center);
	if( dotProduct > sphere.radius ) return PlaneIntersectionType::Front;
	if( dotProduct < -sphere.radius ) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}
//-----------------------------------------------------------------------------
// TODO: delete
#if 0
[[nodiscard]] inline glm::vec4 Transform(const glm::vec4& vector, const glm::mat4& matrix) noexcept
{
	return {
		(vector.x * matrix[0][0]) + (vector.y * matrix[1][0]) + (vector.z * matrix[2][0]) + (vector.w * matrix[3][0]),
		(vector.x * matrix[0][1]) + (vector.y * matrix[1][1]) + (vector.z * matrix[2][1]) + (vector.w * matrix[3][1]),
		(vector.x * matrix[0][2]) + (vector.y * matrix[1][2]) + (vector.z * matrix[2][2]) + (vector.w * matrix[3][2]),
		(vector.x * matrix[0][3]) + (vector.y * matrix[1][3]) + (vector.z * matrix[2][3]) + (vector.w * matrix[3][3]),
	};
}
#endif
//-----------------------------------------------------------------------------
Plane Plane::Transform(const Plane& plane, const glm::mat4& matrix)
{
	const auto vector = plane.ToVector4();
#if 0
	const auto transformMatrix = glm::inverse(matrix);
	const auto transformedVector = ::Transform(vector, transformMatrix);
	return Plane(transformedVector);
#else
	// TODO: проверить
	// влияет левостороняя или правостороняя система координат?
	// а порядок умножения вектора на матрицу?
	const auto transformMatrix = glm::transpose(glm::inverse(matrix));
	const auto transformedVector = transformMatrix * vector;
	return Plane(transformedVector);
#endif
}
//-----------------------------------------------------------------------------
Plane Plane::CreateFromPointNormal(const glm::vec3& point, const glm::vec3& normal)
{
	return Plane(normal, -glm::dot(normal, point));
}
//-----------------------------------------------------------------------------