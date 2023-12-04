#include "stdafx.h"
#include "BoundingSphere.h"
#include "Core/Math/MathLib.h"
#include "Triangle.h"
#include "BoundingAABB.h"
#include "BoundingOrientedBox.h"
#include "Plane.h"
#include "Ray.h"
//-----------------------------------------------------------------------------
constexpr float SphereEnlargeFactor = 1e-6f; // avoid floating error
//-----------------------------------------------------------------------------
BoundingSphere::BoundingSphere(const glm::vec3& a, const glm::vec3& b) noexcept
{
	center = (a + b) * 0.5f;
	radius = glm::length(a - b) * 0.5f + SphereEnlargeFactor;
}
//-----------------------------------------------------------------------------
BoundingSphere::BoundingSphere(const glm::vec3* points, size_t count) noexcept
{
	if (count == 0) return;

	for (size_t i = 0; i < count; i++)
		center += points[i];

	center /= (float)count;

	float maxDist = 0.0f;
	for (size_t i = 0; i < count; i++)
	{
		float dist = glm::distance(points[i], center); // TODO: ???
		if (dist > maxDist)
			maxDist = dist;
	}

	radius = maxDist;
}
//-----------------------------------------------------------------------------
BoundingSphere::BoundingSphere(const glm::vec3* points, size_t count, const glm::vec3& inCenter) noexcept
{
	if (count == 0) return;

	center = inCenter;

	for (size_t i = 0; i < count; i++)
		center += points[i];

	center /= (float)count;

	float maxDistSq = 0.0f;
	for (size_t i = 0; i < count; i++)
	{
		float dist = glm::length(points[i] - center); // TODO: ???
		if (dist > maxDistSq)
			maxDistSq = dist;
	}

	radius = maxDistSq;
}
//-----------------------------------------------------------------------------
void BoundingSphere::Merge(const BoundingSphere& other) noexcept
{
	float distance = glm::distance(other.center,center);

	if (distance > radius + other.radius)
		return;

	if (distance <= radius - other.radius)
	{
		center = other.center;
		radius = other.radius;
		return;
	}

	if (distance <= other.radius - radius)
		return;

	float half = (distance + radius + other.radius) * 0.5f;
	float scale = half / distance;
	center = (center + other.center) * scale;
	radius = half;
}
//-----------------------------------------------------------------------------
void BoundingSphere::Merge(const glm::vec3& point) noexcept
{
	float distance = glm::distance(point, center);

	if (distance > radius)
		return;

	if (distance <= 0.0f)
	{
		center = point;
		radius = 0.0f;
		return;
	}

	float half = (distance + radius) * 0.5f;
	float scale = half / distance;
	center = (center + point) * scale;
	radius = half;
}
//-----------------------------------------------------------------------------
void BoundingSphere::Merge(const glm::vec3* points, size_t count) noexcept
{
	if (count == 0)
		return;

	float tempRadius = 0.0f;
	glm::vec3 tempCenter = points[0];

	for (size_t i = 1; i < count; i++)
	{
		float distance = glm::distance(points[i], tempCenter);

		if (distance > tempRadius)
			tempRadius = distance;

		tempCenter += points[i];
	}

	tempCenter /= (float)count;

	float distance = glm::distance(tempCenter, center);

	if (distance > radius)
		return;

	if (distance <= 0.0f)
	{
		center = tempCenter;
		radius = 0.0f;
		return;
	}

	float half = (distance + radius + tempRadius) * 0.5f;
	float scale = half / distance;
	center = (center + tempCenter) * scale;
	radius = half;
}
//-----------------------------------------------------------------------------
void BoundingSphere::Transform(const glm::mat4& transform) noexcept
{
	center = transform * glm::vec4(center, 1.0f);
}
//-----------------------------------------------------------------------------
void BoundingSphere::Transform(float scale, const glm::quat& rotation, const glm::vec3& translation) noexcept
{
	glm::mat4 transform = glm::mat4(1.0f);
	// TODO: провести все операции трансформации над матрицей
	assert(0);
	Transform(transform);
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const glm::vec3& point) const noexcept
{
	const auto distanceSquared = DistanceSquared(point, center);
	const auto radiusSquared = radius * radius;
	if (distanceSquared > radiusSquared) return ContainmentType::Disjoint;
	else if (distanceSquared < radiusSquared) return ContainmentType::Contains;
	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const Triangle& tri) const noexcept
{
	assert(0); // TODO: нереализовано
	return {};
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const noexcept
{
	const auto distance = glm::distance(center, sphere.center);
	if (distance > radius + sphere.radius) return ContainmentType::Disjoint;
	if (distance + sphere.radius < radius) return ContainmentType::Contains;
	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const BoundingAABB& box) const noexcept
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
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const BoundingOrientedBox& box) const noexcept
{
	assert(0); // TODO: нереализовано
	return ContainmentType();
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const BoundingFrustum& fr) const noexcept
{
	assert(0); // TODO: нереализовано
	return ContainmentType();
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const glm::vec3& point) const noexcept
{
	return glm::distance2(point, center) <= radius * radius;
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const Triangle& tri) const noexcept
{
	assert(0); // TODO: нереализовано
	return false;
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto distance = glm::distance(center, sphere.center);
	return distance <= radius + sphere.radius;
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const BoundingAABB& aabb) const noexcept
{
	return aabb.Intersects(*this);
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const BoundingOrientedBox& box) const noexcept
{
	assert(0); // TODO: Not Impl
	return false;
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const BoundingFrustum& fr) const noexcept
{
	assert(0); // TODO: Not Impl
	return false;
}
//-----------------------------------------------------------------------------
PlaneIntersectionType BoundingSphere::Intersects(const Plane& plane) const noexcept
{
	return plane.Intersects(*this);
}
//-----------------------------------------------------------------------------
std::optional<float> BoundingSphere::Intersects(const Ray& ray) const noexcept
{
	return ray.Intersects(*this);
}
//-----------------------------------------------------------------------------
std::optional<CollisionHit> BoundingSphere::Hit(const BoundingSphere& sphere) const noexcept
{
	const glm::vec3 d = sphere.center - center;
	const float r = radius + sphere.radius;
	const float d2 = glm::dot(d, d);
	if (d2 > r * r) return std::nullopt;

	const float l = sqrtf(d2);
	const float linv = 1.0f / ((l != 0.0f) ? l : 1.0f);

	CollisionHit hit;
	hit.normal = d * linv;
	hit.depth = r - l;
	hit.contactPoint = sphere.center - hit.normal * sphere.radius;
	return hit;
}
//-----------------------------------------------------------------------------