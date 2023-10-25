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

[[nodiscard]] inline float LengthSquared(const glm::vec3& v) noexcept
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

[[nodiscard]] inline float DistanceSquared(const glm::vec3& a, const glm::vec3& b) noexcept
{
	return LengthSquared(a - b);
}