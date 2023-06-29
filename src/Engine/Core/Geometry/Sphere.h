#pragma once

class Sphere
{
public:
	Sphere() = default;
	Sphere(Sphere&&) = default;
	Sphere(const Sphere&) = default;
	constexpr Sphere(const glm::vec3& Center, float Radius) noexcept
		: center(Center), radius(Radius) {}

	Sphere& operator=(Sphere&&) = default;
	Sphere& operator=(const Sphere&) = default;

	glm::vec3 center = glm::vec3(0.0f);
	float radius = 0.0f;
};