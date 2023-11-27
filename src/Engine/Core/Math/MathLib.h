#pragma once

constexpr float EPSILON = 0.000001f;

namespace Math
{
	bool Compare(float f1, float f2);
} // namespace Math

#include "Core/Math/MathLib.inl"


// get normal of triangle v1-v2-v3 with left handed winding
inline glm::vec3 GetNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	const glm::vec3 vA = v3 - v1;
	const glm::vec3 vB = v2 - v1;
	return glm::normalize(glm::cross(vB, vA));
}

// returns the barycentric coordinates of point p within triangle t0-t1-t2 with the result packed into a vec (u = x, v = y, w = z)
inline glm::vec3 GetBarycentric(const glm::vec3& p, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2)
{
	glm::vec3 v0 = t1 - t0;
	glm::vec3 v1 = t2 - t0;
	glm::vec3 v2 = p - t0;
	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d11 = glm::dot(v1, v1);
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	return { u, v, w };
}

[[nodiscard]] inline float LengthSquared(const glm::vec3& v) noexcept
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

[[nodiscard]] inline float DistanceSquared(const glm::vec3& a, const glm::vec3& b) noexcept
{
	return LengthSquared(a - b);
}