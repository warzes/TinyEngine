#include "stdafx.h"
#include "Ray.h"
#include "BoundingBox.h"
#include "BoundingFrustum.h"
#include "BoundingSphere.h"
#include "Plane.h"
#include "Core/Math/MathCoreFunc.h"
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingBox& box) const
{
	using T = float;

	constexpr auto PositiveInfinity = std::numeric_limits<T>::max();
	constexpr auto NegativeInfinity = std::numeric_limits<T>::lowest();
	constexpr auto Epsilon = std::numeric_limits<T>::epsilon();

	auto& ray = *this;

	auto tNear = NegativeInfinity;
	auto tFar = PositiveInfinity;

	if( std::abs(ray.direction.x) < Epsilon )
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

	if( std::abs(ray.direction.y) < Epsilon )
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

	if( std::abs(ray.direction.z) < Epsilon )
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
std::optional<float> Ray::Intersects(const BoundingFrustum& frustum) const
{
	return frustum.Intersects(*this);
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingSphere& sphere) const
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
std::optional<float> Ray::Intersects(const Plane& plane) const
{
	constexpr auto Epsilon = 1e-6f;

	const auto denom = glm::dot(plane.normal, direction);
	if( std::abs(denom) < Epsilon )
	{
		return std::nullopt;
	}

	const auto dot = glm::dot(plane.normal, position) + plane.distance;
	const auto distance = -dot / denom;
	if( distance < 0.0f )
	{
		return std::nullopt;
	}
	return distance;
}
//-----------------------------------------------------------------------------