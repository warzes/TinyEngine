#pragma once

#include "GeometryCore.h"

class Ray final
{
public:
	Ray() noexcept = default;
	Ray(const Ray&) noexcept = default;
	Ray(const glm::vec3& position, const glm::vec3& direction) noexcept : position(position), direction(direction) {}

	Ray& operator=(const Ray&) noexcept = default;

	bool operator==(const Ray& rhs) const noexcept { return position == rhs.position && direction == rhs.direction; }
	bool operator!=(const Ray& rhs) const noexcept { return position != rhs.position || direction != rhs.direction; }

	[[nodiscard]] std::optional<float> Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Plane& plane) const noexcept;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f);
};