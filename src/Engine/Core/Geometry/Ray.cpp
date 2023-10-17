#include "stdafx.h"
#include "Ray.h"
#include "BoundingAABB.h"
#include "BoundingFrustum.h"
#include "BoundingSphere.h"
#include "Plane.h"
#include "Core/Math/MathCoreFunc.h"
//-----------------------------------------------------------------------------
glm::vec3 Ray::ClosestPoint(const Ray& ray) const
{
	// Algorithm based on http://paulbourke.net/geometry/lineline3d/
	const glm::vec3 p13 = position - ray.position;
	const glm::vec3 p43 = ray.direction;
	const glm::vec3 p21 = direction;

	const float d1343 = glm::dot(p13, p43);
	const float d4321 = glm::dot(p43, p21);
	const float d1321 = glm::dot(p13, p21);
	const float d4343 = glm::dot(p43, p43);
	const float d2121 = glm::dot(p21, p21);

	const float d = d2121 * d4343 - d4321 * d4321;
	if (glm::abs(d) < EPSILON)
		return position;
	const float n = d1343 * d4321 - d1321 * d4343;
	const float a = n / d;

	return position + a * direction;
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingAABB& box) const noexcept
{
	using T = float;

	constexpr auto PositiveInfinity = std::numeric_limits<T>::max();
	constexpr auto NegativeInfinity = std::numeric_limits<T>::lowest();

	auto& ray = *this;

	auto tNear = NegativeInfinity;
	auto tFar = PositiveInfinity;

	if( std::abs(ray.direction.x) < EPSILON)
	{
		if( (ray.position.x < box.min.x) || (ray.position.x > box.max.x) )
		{
			return std::nullopt;
		}
	}
	else
	{
		assert(ray.direction.x != 0);
		auto t1 = (box.min.x - ray.position.x) / ray.direction.x;
		auto t2 = (box.max.x - ray.position.x) / ray.direction.x;

		if( t1 > t2 )
		{
			std::swap(t1, t2);
		}

		assert(tNear <= t1);
		assert(tFar >= t2);
		tNear = t1;
		tFar = t2;

		if( tNear > tFar || tFar < 0 )
		{
			return std::nullopt;
		}
	}

	if( std::abs(ray.direction.y) < EPSILON)
	{
		if( (ray.position.y < box.min.y) || (ray.position.y > box.max.y) )
		{
			return std::nullopt;
		}
	}
	else
	{
		assert(ray.direction.y != 0);
		auto t1 = (box.min.y - ray.position.y) / ray.direction.y;
		auto t2 = (box.max.y - ray.position.y) / ray.direction.y;

		if( t1 > t2 )
		{
			std::swap(t1, t2);
		}
		if( tNear < t1 )
		{
			tNear = t1;
		}
		if( tFar > t2 )
		{
			tFar = t2;
		}
		if( tNear > tFar || tFar < 0 )
		{
			return std::nullopt;
		}
	}

	if( std::abs(ray.direction.z) < EPSILON)
	{
		if( (ray.position.z < box.min.z) || (ray.position.z > box.max.z) )
		{
			return std::nullopt;
		}
	}
	else
	{
		assert(ray.direction.z != 0);
		auto t1 = (box.min.z - ray.position.z) / ray.direction.z;
		auto t2 = (box.max.z - ray.position.z) / ray.direction.z;

		if( t1 > t2 )
		{
			std::swap(t1, t2);
		}
		if( tNear < t1 )
		{
			tNear = t1;
		}
		if( tFar > t2 )
		{
			tFar = t2;
		}
		if( tNear > tFar || tFar < 0 )
		{
			return std::nullopt;
		}
	}

	assert(tNear <= tFar && tFar >= 0);
	return tNear;
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingFrustum& frustum) const noexcept
{
	return frustum.Intersects(*this);
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto toSphere = sphere.center - position;
	const auto toSphereLengthSquared = LengthSquared(toSphere);
	const auto sphereRadiusSquared = sphere.radius * sphere.radius;

	if( toSphereLengthSquared < sphereRadiusSquared )
	{
		return 0.0f;
	}

	const auto distance = glm::dot(direction, toSphere);
	if( distance < 0 )
	{
		return std::nullopt;
	}

	const auto discriminant = sphereRadiusSquared + distance * distance - toSphereLengthSquared;
	if( discriminant < 0 )
	{
		return std::nullopt;
	}
	return glm::max(distance - std::sqrt(discriminant), 0.0f);
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const Plane& plane) const noexcept
{
	const auto denom = glm::dot(plane.normal, direction);
	if( std::abs(denom) < EPSILON)
		return std::nullopt;

	const auto dot = glm::dot(plane.normal, position) + plane.distance;
	const auto distance = -dot / denom;
	if( distance < 0.0f )
		return std::nullopt;

	return distance;
}
//-----------------------------------------------------------------------------