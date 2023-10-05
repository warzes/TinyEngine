#pragma once

class Triangle final
{
public:
	Triangle() = default;
	Triangle(const Triangle&) noexcept = default;

	Triangle& operator=(const Triangle&) noexcept = default;

	glm::vec3 p0 = glm::vec3(0.0f);
	glm::vec3 p1 = glm::vec3(0.0f);
	glm::vec3 p2 = glm::vec3(0.0f);
};