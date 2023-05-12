#pragma once

// get normal of triangle v1-v2-v3 with left handed winding
inline glm::vec3 GetNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	const glm::vec3 vA = v3 - v1;
	const glm::vec3 vB = v2 - v1;
	return glm::normalize(glm::cross(vB, vA));
}