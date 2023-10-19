#pragma once

#include "GeometryCore.h"

// Infinite straight line in three-dimensional space.
class Ray final
{
public:
	Ray() noexcept = default;
	Ray(const Ray&) noexcept = default;
	Ray(const glm::vec3& position, const glm::vec3& direction) noexcept : position(position), direction(direction) {}

	Ray& operator=(const Ray&) noexcept = default;

	bool operator==(const Ray& rhs) const noexcept { return position == rhs.position && direction == rhs.direction; }
	bool operator!=(const Ray& rhs) const noexcept { return position != rhs.position || direction != rhs.direction; }

	// Project a point on the ray.
	[[nodiscard]] glm::vec3 Project(const glm::vec3& point) const;

	// Return distance of a point from the ray.
	[[nodiscard]] float GetDistance(const glm::vec3& point) const;

	// Return closest point to another ray.
	[[nodiscard]] glm::vec3 ClosestPoint(const Ray& ray) const;

	[[nodiscard]] std::optional<float> Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Plane& plane) const noexcept;
	// Return hit distance to a triangle and optionally normal, or infinity if no hit.
	[[nodiscard]] std::optional<float> Intersects(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, glm::vec3* outNormal = nullptr) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Triangle& tri, glm::vec3* outNormal = nullptr) const noexcept;

	// Return hit distance to non-indexed geometry data, or infinity if no hit. Optionally return normal.
	[[nodiscard]] std::optional<float> Intersects(const void* vertexData, size_t vertexSize, size_t vertexStart, size_t vertexCount, glm::vec3* outNormal = nullptr) const noexcept;
	// Return hit distance to indexed geometry data, or infinity if no hit.
	[[nodiscard]] std::optional<float> Intersects(const void* vertexData, size_t vertexSize, const void* indexData, size_t indexSize, size_t indexStart, size_t indexCount, glm::vec3* outNormal = nullptr) const;

	// Return whether ray is inside non-indexed geometry.
	[[nodiscard]] bool InsideGeometry(const void* vertexData, size_t vertexSize, size_t vertexStart, size_t vertexCount) const;
	// Return whether ray is inside indexed geometry.
	[[nodiscard]] bool InsideGeometry(const void* vertexData, size_t vertexSize, const void* indexData, size_t indexSize, size_t indexStart, size_t indexCount) const;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f);
};