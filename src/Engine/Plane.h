#pragma once

#include "GeometryCore.h"

class Plane final
{
public:
	Plane() = default;
	Plane(const glm::vec3& normal, float distance) : normal(normal), distance(distance) {}
	Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2) noexcept;

	[[nodiscard]] float DotCoordinate(const glm::vec3& vec) const noexcept;

	[[nodiscard]] PlaneIntersectionType Intersects(const glm::vec3& point) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingBox& box) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingSphere& sphere) const noexcept;

	glm::vec3 normal = glm::vec3(0.0f);
	float distance = 0.0f;
};