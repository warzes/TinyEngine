#pragma once

#include "GeometryCore.h"

// BoundingBox is an axis-aligned bounding box in 3D space.
// Rename AABB???
class BoundingBox final
{
public:
	static constexpr int CornerCount = 8;

	BoundingBox() noexcept = default;
	BoundingBox(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingBox& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] bool Intersects(const BoundingBox& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::array<glm::vec3, CornerCount> GetCorners() const noexcept;

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };
};