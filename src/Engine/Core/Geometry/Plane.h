#pragma once

#include "GeometryCore.h"

class Plane final
{
public:
	Plane() = default;
	Plane(const Plane&) noexcept = default;
	Plane(const glm::vec3& normal, float distance) noexcept : normal(normal), distance(distance) {}
	Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2) noexcept;
	Plane(const Triangle& tri) noexcept;

	Plane& operator=(const Plane&) noexcept = default;

	void Normalize() noexcept;
	[[nodiscard]] static Plane Normalize(const Plane& plane) noexcept;

	[[nodiscard]] float Dot(const glm::vec4& vec) const noexcept;
	[[nodiscard]] float DotCoordinate(const glm::vec3& vec) const noexcept;
	[[nodiscard]] float DotNormal(const glm::vec3& vec) const noexcept;

	[[nodiscard]] float GetDistanceToPoint(const glm::vec3& point) const noexcept;

	[[nodiscard]] PlaneIntersectionType Intersects(const glm::vec3& point) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] static Plane Transform(const Plane& plane, const glm::mat4& matrix);

	[[nodiscard]] static Plane CreateFromPointNormal(const glm::vec3& point, const glm::vec3& normal);

	glm::vec3 normal = glm::vec3(0.0f); // Normal vector of the plane.
	float distance = 0.0f; // Signed distance to the origin of the coordinate system.
};