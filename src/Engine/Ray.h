#pragma once

#include "GeometryCore.h"

class Ray final
{
public:
	Ray() = default;
	Ray(const glm::vec3& position, const glm::vec3& direction) : position(position), direction(direction) {}

	[[nodiscard]] std::optional<float> Intersects(const BoundingBox& box) const;
	[[nodiscard]] std::optional<float> Intersects(const BoundingFrustum& frustum) const;
	[[nodiscard]] std::optional<float> Intersects(const BoundingSphere& sphere) const;
	[[nodiscard]] std::optional<float> Intersects(const Plane& plane) const;

	glm::vec3 position;
	glm::vec3 direction;
};