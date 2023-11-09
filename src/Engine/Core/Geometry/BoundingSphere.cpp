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
void BoundingSphere::Transform(BoundingSphere& Out, const glm::mat4& mat) noexcept
{
	assert(0); // TODO: Not Impl
}
//-----------------------------------------------------------------------------
void BoundingSphere::Transform(BoundingSphere& out, float scale, const glm::quat& rotation, const glm::vec3& translation) noexcept
{
	assert(0); // TODO: Not Impl
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
	assert(0); // TODO: Not Impl
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
	assert(0); // TODO: Not Impl
	return ContainmentType();
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(const BoundingFrustum& fr) const noexcept
{
	assert(0); // TODO: Not Impl
	return ContainmentType();
}
//-----------------------------------------------------------------------------
ContainmentType BoundingSphere::ContainedBy(const Plane& plane0, const Plane& plane1, const Plane& plane2, const Plane& Plane3, const Plane& Plane4, const Plane& Plane5) const noexcept
{
	assert(0); // TODO: Not Impl
	return ContainmentType();
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const Triangle& tri) const noexcept
{
	assert(0); // TODO: Not Impl
	return false;
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto distance = glm::distance(center, sphere.center);
	return distance <= radius + sphere.radius;
}
//-----------------------------------------------------------------------------
bool BoundingSphere::Intersects(const BoundingAABB& box) const noexcept
{
	return box.Intersects(*this);
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