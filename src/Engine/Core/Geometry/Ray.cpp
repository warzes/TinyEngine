#include "stdafx.h"
#include "Ray.h"
#include "BoundingAABB.h"
#include "BoundingFrustum.h"
#include "BoundingSphere.h"
#include "Plane.h"
#include "Core/Math/MathLib.h"
#include "Triangle.h"
//-----------------------------------------------------------------------------
glm::vec3 Ray::Project(const glm::vec3& point) const
{
	const glm::vec3 offset = point - position;
	return position + glm::dot(offset, direction) * direction;
}
//-----------------------------------------------------------------------------
float Ray::GetDistance(const glm::vec3& point) const
{
	const glm::vec3 projected = Project(point);
	return glm::length(point - projected);
}
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
	constexpr auto PositiveInfinity = std::numeric_limits<float>::max();
	constexpr auto NegativeInfinity = std::numeric_limits<float>::lowest();

	auto tNear = NegativeInfinity;
	auto tFar = PositiveInfinity;

	if( std::abs(direction.x) < EPSILON)
	{
		if( (position.x < box.min.x) || (position.x > box.max.x) )
			return std::nullopt;
	}
	else
	{
		assert(direction.x != 0);
		auto t1 = (box.min.x - position.x) / direction.x;
		auto t2 = (box.max.x - position.x) / direction.x;

		if( t1 > t2 )
			std::swap(t1, t2);

		assert(tNear <= t1);
		assert(tFar >= t2);
		tNear = t1;
		tFar = t2;

		if( tNear > tFar || tFar < 0 )
			return std::nullopt;
	}

	if( std::abs(direction.y) < EPSILON)
	{
		if( (position.y < box.min.y) || (position.y > box.max.y) )
			return std::nullopt;
	}
	else
	{
		assert(direction.y != 0);
		auto t1 = (box.min.y - position.y) / direction.y;
		auto t2 = (box.max.y - position.y) / direction.y;

		if( t1 > t2 )
			std::swap(t1, t2);
		if( tNear < t1 )
			tNear = t1;
		if( tFar > t2 )
			tFar = t2;
		if( tNear > tFar || tFar < 0 )
			return std::nullopt;
	}

	if( std::abs(direction.z) < EPSILON)
	{
		if( (position.z < box.min.z) || (position.z > box.max.z) )
			return std::nullopt;
	}
	else
	{
		assert(direction.z != 0);
		auto t1 = (box.min.z - position.z) / direction.z;
		auto t2 = (box.max.z - position.z) / direction.z;

		if( t1 > t2 )
			std::swap(t1, t2);
		if( tNear < t1 )
			tNear = t1;
		if( tFar > t2 )
			tFar = t2;
		if( tNear > tFar || tFar < 0 )
			return std::nullopt;
	}

	assert(tNear <= tFar && tFar >= 0);
	return tNear;
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingFrustum& frustum) const noexcept
{
	assert(0); // TODO: не реализовано
	return {};
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const BoundingSphere& sphere) const noexcept
{
	const auto toSphere = sphere.center - position;
	const auto toSphereLengthSquared = LengthSquared(toSphere);
	const auto sphereRadiusSquared = sphere.radius * sphere.radius;

	// Check if ray originates inside the sphere
	if( toSphereLengthSquared < sphereRadiusSquared )
		return 0.0f;

	const auto distance = glm::dot(direction, toSphere);
	if( distance < 0.0f )
		return std::nullopt;

	const auto discriminant = sphereRadiusSquared + distance * distance - toSphereLengthSquared;
	if( discriminant < 0.0f)
		return std::nullopt;

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
std::optional<float> Ray::Intersects(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, glm::vec3* outNormal) const noexcept
{
	// Based on Fast, Minimum Storage Ray/Triangle Intersection by Mцller & Trumbore
	// http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
	// Calculate edge vectors
	glm::vec3 edge1(v1 - v0);
	glm::vec3 edge2(v2 - v0);

	// Calculate determinant & check backfacing
	glm::vec3 p(glm::cross(direction, edge2));
	float det = glm::dot(edge1, p);
	if (det >= EPSILON)
	{
		// Calculate u & v parameters and test
		glm::vec3 t(position - v0);
		float u = glm::dot(t, p);
		if (u >= 0.0f && u <= det)
		{
			glm::vec3 q(glm::cross(t, edge1));
			float v = glm::dot(direction, q);
			if (v >= 0.0f && u + v <= det)
			{
				float distance = glm::dot(edge2, q) / det;
				if (distance >= 0.0f)
				{
					// There is an intersection, so calculate distance & optional normal
					if (outNormal)
						*outNormal = glm::cross(edge1, edge2);

					return distance;
				}
			}
		}
	}

	return std::nullopt;
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const Triangle& tri, glm::vec3* outNormal) const noexcept
{
	return Intersects(tri.p0, tri.p1, tri.p2, outNormal);
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const void* vertexData, size_t vertexSize, size_t vertexStart, size_t vertexCount, glm::vec3* outNormal) const noexcept
{
	float nearest = std::numeric_limits<float>::infinity();
	const unsigned char* vertices = ((const unsigned char*)vertexData) + vertexStart * vertexSize;
	size_t index = 0;

	while (index + 2 < vertexCount)
	{
		const glm::vec3& v0 = *((const glm::vec3*)(&vertices[index * vertexSize]));
		const glm::vec3& v1 = *((const glm::vec3*)(&vertices[(index + 1) * vertexSize]));
		const glm::vec3& v2 = *((const glm::vec3*)(&vertices[(index + 2) * vertexSize]));

		auto ret = Intersects(v0, v1, v2, outNormal);
		if (ret) nearest = glm::min(nearest, ret.value());
		index += 3;
	}

	// TODO: что вернет, если nearest равен infinity() и как это проверять
	return nearest;
}
//-----------------------------------------------------------------------------
std::optional<float> Ray::Intersects(const void* vertexData, size_t vertexSize, const void* indexData, size_t indexSize, size_t indexStart, size_t indexCount, glm::vec3* outNormal) const
{
	float nearest = std::numeric_limits<float>::infinity();
	const unsigned char* vertices = (const unsigned char*)vertexData;

	// 16-bit indices
	if (indexSize == sizeof(unsigned short))
	{
		const unsigned short* indices = ((const unsigned short*)indexData) + indexStart;
		const unsigned short* indicesEnd = indices + indexCount;

		while (indices < indicesEnd)
		{
			const glm::vec3& v0 = *((const glm::vec3*)(&vertices[indices[0] * vertexSize]));
			const glm::vec3& v1 = *((const glm::vec3*)(&vertices[indices[1] * vertexSize]));
			const glm::vec3& v2 = *((const glm::vec3*)(&vertices[indices[2] * vertexSize]));
			auto ret = Intersects(v0, v1, v2, outNormal);
			if (ret) nearest = glm::min(nearest, ret.value());
			indices += 3;
		}
	}
	// 32-bit indices
	else
	{
		const size_t* indices = ((const size_t*)indexData) + indexStart;
		const size_t* indicesEnd = indices + indexCount;

		while (indices < indicesEnd)
		{
			const glm::vec3& v0 = *((const glm::vec3*)(&vertices[indices[0] * vertexSize]));
			const glm::vec3& v1 = *((const glm::vec3*)(&vertices[indices[1] * vertexSize]));
			const glm::vec3& v2 = *((const glm::vec3*)(&vertices[indices[2] * vertexSize]));
			auto ret = Intersects(v0, v1, v2, outNormal);
			if (ret) nearest = glm::min(nearest, ret.value());
			indices += 3;
		}
	}

	return nearest;
}
//-----------------------------------------------------------------------------
bool Ray::InsideGeometry(const void* vertexData, size_t vertexSize, size_t vertexStart, size_t vertexCount) const
{
	constexpr float infinity = std::numeric_limits<float>::infinity();
	float currentFrontFace = infinity;
	float currentBackFace = infinity;
	const unsigned char* vertices = ((const unsigned char*)vertexData) + vertexStart * vertexSize;
	size_t index = 0;

	while (index + 2 < vertexCount)
	{
		const glm::vec3& v0 = *((const glm::vec3*)(&vertices[index * vertexSize]));
		const glm::vec3& v1 = *((const glm::vec3*)(&vertices[(index + 1) * vertexSize]));
		const glm::vec3& v2 = *((const glm::vec3*)(&vertices[(index + 2) * vertexSize]));

		auto ret1 = Intersects(v0, v1, v2);
		auto ret2 = Intersects(v2, v1, v0);

		float frontFaceDistance = ret1 ? ret1.value() : 0.0f;
		float backFaceDistance = ret2 ? ret2.value() : 0.0f;
		currentFrontFace = glm::min(frontFaceDistance > 0.0f ? frontFaceDistance : infinity, currentFrontFace);
		// A backwards face is just a regular one, with the vertices in the opposite order. This essentially checks backfaces by
		// checking reversed frontfaces
		currentBackFace = glm::min(backFaceDistance > 0.0f ? backFaceDistance : infinity, currentBackFace);
		index += 3;
	}

	// If the closest face is a backface, that means that the ray originates from the inside of the geometry
	// NOTE: there may be cases where both are equal, as in, no collision to either. This is prevented in the most likely case
	// (ray doesnt hit either) by this conditional
	if (currentFrontFace != infinity || currentBackFace != infinity)
		return currentBackFace < currentFrontFace;

	// It is still possible for two triangles to be equally distant from the triangle, however, this is extremely unlikely.
	// As such, it is safe to assume they are not
	return false;
}
//-----------------------------------------------------------------------------
bool Ray::InsideGeometry(const void* vertexData, size_t vertexSize, const void* indexData, size_t indexSize, size_t indexStart, size_t indexCount) const
{
	constexpr float infinity = std::numeric_limits<float>::infinity();
	float currentFrontFace = infinity;
	float currentBackFace = infinity;
	const unsigned char* vertices = (const unsigned char*)vertexData;

	// 16-bit indices
	if (indexSize == sizeof(unsigned short))
	{
		const unsigned short* indices = ((const unsigned short*)indexData) + indexStart;
		const unsigned short* indicesEnd = indices + indexCount;

		while (indices < indicesEnd)
		{
			const glm::vec3& v0 = *((const glm::vec3*)(&vertices[indices[0] * vertexSize]));
			const glm::vec3& v1 = *((const glm::vec3*)(&vertices[indices[1] * vertexSize]));
			const glm::vec3& v2 = *((const glm::vec3*)(&vertices[indices[2] * vertexSize]));

			auto ret1 = Intersects(v0, v1, v2);
			auto ret2 = Intersects(v2, v1, v0);
			float frontFaceDistance = ret1 ? ret1.value() : 0.0f;
			float backFaceDistance = ret2 ? ret2.value() : 0.0f;

			currentFrontFace = glm::min(frontFaceDistance > 0.0f ? frontFaceDistance : infinity, currentFrontFace);
			// A backwards face is just a regular one, with the vertices in the opposite order. This essentially checks backfaces by
			// checking reversed frontfaces
			currentBackFace = glm::min(backFaceDistance > 0.0f ? backFaceDistance : infinity, currentBackFace);
			indices += 3;
		}
	}
	// 32-bit indices
	else
	{
		const size_t* indices = ((const size_t*)indexData) + indexStart;
		const size_t* indicesEnd = indices + indexCount;

		while (indices < indicesEnd)
		{
			const glm::vec3& v0 = *((const glm::vec3*)(&vertices[indices[0] * vertexSize]));
			const glm::vec3& v1 = *((const glm::vec3*)(&vertices[indices[1] * vertexSize]));
			const glm::vec3& v2 = *((const glm::vec3*)(&vertices[indices[2] * vertexSize]));

			auto ret1 = Intersects(v0, v1, v2);
			auto ret2 = Intersects(v2, v1, v0);
			float frontFaceDistance = ret1 ? ret1.value() : 0.0f;
			float backFaceDistance = ret2 ? ret2.value() : 0.0f;
			currentFrontFace = glm::min(frontFaceDistance > 0.0f ? frontFaceDistance : infinity, currentFrontFace);
			// A backwards face is just a regular one, with the vertices in the opposite order. This essentially checks backfaces by
			// checking reversed frontfaces
			currentBackFace = glm::min(backFaceDistance > 0.0f ? backFaceDistance : infinity, currentBackFace);
			indices += 3;
		}
	}

	// If the closest face is a backface, that means that the ray originates from the inside of the geometry
	// NOTE: there may be cases where both are equal, as in, no collision to either. This is prevented in the most likely case
	// (ray doesnt hit either) by this conditional
	if (currentFrontFace != infinity || currentBackFace != infinity)
		return currentBackFace < currentFrontFace;

	// It is still possible for two triangles to be equally distant from the triangle, however, this is extremely unlikely.
	// As such, it is safe to assume they are not
	return false;
}
//-----------------------------------------------------------------------------