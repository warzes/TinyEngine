#pragma once

#include "Core/Geometry/GeometryCore.h"

// Surface in three-dimensional space.
class Plane final
{
public:
	// Plane at origin with normal pointing up.
	static const Plane Up;

	Plane() noexcept = default;
	Plane(Plane&&) noexcept = default;
	Plane(const Plane&) noexcept = default;
	Plane(const glm::vec3& normal, float distance) noexcept : normal(normal), distance(distance) {}
	Plane(const glm::vec3& normal, const glm::vec3& point) noexcept;
	Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2) noexcept;
	Plane(const Triangle& tri) noexcept;
	Plane(const glm::vec4& plane) noexcept;

	Plane& operator=(Plane&&) noexcept = default;
	Plane& operator=(const Plane&) noexcept = default;

	[[nodiscard]] glm::vec4 ToVector4() const noexcept { return glm::vec4(normal, distance); }

	[[nodiscard]] glm::vec3 GetOrigin() const noexcept;

	void Normalize() noexcept;
	[[nodiscard]] static Plane Normalize(const Plane& plane) noexcept;

	[[nodiscard]] float Dot(const glm::vec4& vec) const noexcept;
	[[nodiscard]] float DotCoordinate(const glm::vec3& vec) const noexcept;
	[[nodiscard]] float DotNormal(const glm::vec3& vec) const noexcept;

	// Project a point on the plane.
	[[nodiscard]] glm::vec3 Project(const glm::vec3& point) const noexcept;

	// Return signed distance to a point.
	[[nodiscard]] float GetDistanceToPoint(const glm::vec3& point) const noexcept;

	// Reflect a normalized direction vector.
	[[nodiscard]] glm::vec3 Reflect(const glm::vec3& direction) const noexcept;

	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const glm::vec3& point) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const OldBoundingFrustum& frustum) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] static Plane Transform(const Plane& plane, const glm::mat4& matrix);

	[[nodiscard]] static Plane CreateFromPointNormal(const glm::vec3& point, const glm::vec3& normal);

	glm::vec3 normal = glm::vec3(0.0f); // Normal vector of the plane.
	float distance = 0.0f;              // Signed distance to the origin of the coordinate system.
};