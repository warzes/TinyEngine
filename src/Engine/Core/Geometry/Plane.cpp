#include "stdafx.h"
#include "Plane.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"
#include "BoundingFrustum.h"
#include "Triangle.h"
#include "Ray.h"
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
	const auto edge1 = point1 - point0;
	const auto edge2 = point2 - point0;
	normal = glm::normalize(glm::cross(edge1, edge2));
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
void Plane::Transform(const glm::mat4& transform) noexcept
{
	glm::vec4 plane = glm::vec4(normal, distance);
	plane = transform * plane;
	normal = glm::vec3(plane.x, plane.y, plane.z);
	distance = plane.w;
}
//-----------------------------------------------------------------------------
Plane Plane::Transformed(const glm::mat4& transform) const noexcept
{
	glm::vec4 plane = glm::vec4(normal, distance);
	plane = transform * plane;
	return Plane(glm::vec3(plane.x, plane.y, plane.z), plane.w);
}
//-----------------------------------------------------------------------------
void Plane::Normalize() noexcept
{
	const auto length = glm::length(normal);

	if (length >= std::numeric_limits<float>::epsilon())
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
glm::vec3 Plane::Reflect(const glm::vec3& direction) const noexcept
{
	return direction - (2.0f * glm::dot(normal, direction) * normal);
}
//-----------------------------------------------------------------------------
float Plane::Distance(const glm::vec3& point) const noexcept
{
	return glm::dot(normal, point) + distance;
}
//-----------------------------------------------------------------------------
bool Plane::IsPointOnPlane(const glm::vec3& point) const noexcept
{
	return Distance(point) >= -EPSILON;
}
//-----------------------------------------------------------------------------
std::optional<float> Plane::Intersects(const Ray& ray) const noexcept
{
	return ray.Intersects(*this);
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const glm::vec3& point) const noexcept
{
	const auto dotProduct = Distance(point);
	if (dotProduct > 0.0f) return PlaneIntersectionType::Front;
	if (dotProduct < 0.0f) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const BoundingAABB& box) const noexcept
{
	glm::vec3 positiveVertex{ box.min.x, box.min.y, box.min.z };
	glm::vec3 negativeVertex{ box.max.x, box.max.y, box.max.z };

	if (normal.x >= 0.0f)
	{
		positiveVertex.x = box.max.x;
		negativeVertex.x = box.min.x;
	}
	if (normal.y >= 0.0f)
	{
		positiveVertex.y = box.max.y;
		negativeVertex.y = box.min.y;
	}
	if (normal.z >= 0.0f)
	{
		positiveVertex.z = box.max.z;
		negativeVertex.z = box.min.z;
	}

	if (Distance(negativeVertex) > 0.0f) return PlaneIntersectionType::Front;
	if (Distance(positiveVertex) < 0.0f) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const BoundingFrustum& frustum) const noexcept
{
	assert(0); // TODO: не реализовано
	return {};
}
//-----------------------------------------------------------------------------
PlaneIntersectionType Plane::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto dotProduct = Distance(sphere.center);
	if (dotProduct > sphere.radius) return PlaneIntersectionType::Front;
	if (dotProduct < -sphere.radius) return PlaneIntersectionType::Back;
	return PlaneIntersectionType::Intersecting;
}
//-----------------------------------------------------------------------------
glm::vec3 Plane::GetOrigin() const noexcept
{
	return -(normal * distance);
}
//-----------------------------------------------------------------------------