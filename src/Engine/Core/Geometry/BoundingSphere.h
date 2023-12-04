#pragma once

#include "Core/Geometry/GeometryCore.h"

class BoundingSphere final
{
public:
	BoundingSphere() noexcept = default;
	BoundingSphere(BoundingSphere&&) noexcept = default;
	BoundingSphere(const BoundingSphere&) noexcept = default;
	BoundingSphere(const glm::vec3& centerSphere, float radiusSphere) : center(centerSphere), radius(radiusSphere) {}
	BoundingSphere(const glm::vec3& min, const glm::vec3& max) noexcept;
	BoundingSphere(const glm::vec3* points, size_t count) noexcept;
	BoundingSphere(const glm::vec3* points, size_t count, const glm::vec3& center) noexcept;

	BoundingSphere& operator=(BoundingSphere&&) noexcept = default;
	BoundingSphere& operator=(const BoundingSphere&) noexcept = default;

	bool operator==(const BoundingSphere& rhs) const { return center == rhs.center && radius == rhs.radius; }
	bool operator!=(const BoundingSphere& rhs) const { return center != rhs.center || radius != rhs.radius; }

	void Merge(const BoundingSphere& other) noexcept;
	void Merge(const glm::vec3& point) noexcept;
	void Merge(const glm::vec3* points, size_t count) noexcept;

	void Transform(const glm::mat4& transform) noexcept;
	void Transform(float scale, const glm::quat& rotation, const glm::vec3& translation) noexcept; // TODO: нереализовано

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const Triangle& tri) const noexcept; // TODO: нереализовано
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept; // TODO: нереализовано
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& fr) const noexcept; // TODO: нереализовано

	[[nodiscard]] bool Intersects(const glm::vec3& point) const noexcept;
	[[nodiscard]] bool Intersects(const Triangle& tri) const noexcept;  // TODO: нереализовано
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept;  // TODO: нереализовано
	[[nodiscard]] bool Intersects(const BoundingFrustum& fr) const noexcept;  // TODO: нереализовано
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::optional<CollisionHit> Hit(const glm::vec3& point) const noexcept;  // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const Triangle& tri) const noexcept;  // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingAABB& aabb) const noexcept;  // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingOrientedBox& box) const noexcept;  // TODO: нереализовано
	
	glm::vec3 center = glm::vec3(0.0f);
	float radius = 0.0f;
};