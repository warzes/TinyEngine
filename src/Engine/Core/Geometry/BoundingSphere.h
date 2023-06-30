#pragma once

#include "GeometryCore.h"

class BoundingSphere final
{
public:
	BoundingSphere() = default;
	BoundingSphere(const BoundingSphere&) noexcept = default;
	BoundingSphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}

	BoundingSphere& operator=(const BoundingSphere&) noexcept = default;

	bool operator==(const BoundingSphere& rhs) const { return center == rhs.center && radius == rhs.radius; }
	bool operator!=(const BoundingSphere& rhs) const { return center != rhs.center || radius != rhs.radius; }

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingBox& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingBox& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] static BoundingSphere CreateFromPoints(const glm::vec3* points, std::size_t pointCount) noexcept;
	[[nodiscard]] static BoundingSphere CreateFromPoints(std::function<glm::vec3(std::size_t)> points, std::size_t pointCount) noexcept;

	glm::vec3 center = glm::vec3(0.0f);
	float radius = 0.0f;
};