#pragma once

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

// TODO: delete
[[nodiscard]] inline glm::vec4 Transform(const glm::vec4& vector, const glm::mat4& matrix) noexcept
{
    return {
        (vector.x * matrix[0][0]) + (vector.y * matrix[1][0]) + (vector.z * matrix[2][0]) + (vector.w * matrix[3][0]),
        (vector.x * matrix[0][1]) + (vector.y * matrix[1][1]) + (vector.z * matrix[2][1]) + (vector.w * matrix[3][1]),
        (vector.x * matrix[0][2]) + (vector.y * matrix[1][2]) + (vector.z * matrix[2][2]) + (vector.w * matrix[3][2]),
        (vector.x * matrix[0][3]) + (vector.y * matrix[1][3]) + (vector.z * matrix[2][3]) + (vector.w * matrix[3][3]),
    };
}