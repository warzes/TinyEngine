#pragma once

#include "Core/Geometry/GeometryCore.h"

class BoundingSphere final
{
public:
	BoundingSphere() noexcept = default;
	BoundingSphere(BoundingSphere&&) noexcept = default;
	BoundingSphere(const BoundingSphere&) noexcept = default;
	BoundingSphere(float x, float y, float z, float radiusSphere) : center({x, y, z}), radius(radiusSphere) {}
	BoundingSphere(const glm::vec3& centerSphere, float radiusSphere) : center(centerSphere), radius(radiusSphere) {}
	BoundingSphere(const glm::vec3& min, const glm::vec3& max) noexcept;

	BoundingSphere& operator=(BoundingSphere&&) noexcept = default;
	BoundingSphere& operator=(const BoundingSphere&) noexcept = default;

	bool operator==(const BoundingSphere& rhs) const { return center == rhs.center && radius == rhs.radius; }
	bool operator!=(const BoundingSphere& rhs) const { return center != rhs.center || radius != rhs.radius; }

	// Transform a sphere by an angle preserving transform.
	static void Transform(BoundingSphere& Out, const glm::mat4& mat) noexcept; // TODO:
	static void Transform(BoundingSphere& Out, float scale, const glm::quat& rotation, const glm::vec3& translation) noexcept; // TODO:

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const Triangle& tri) const noexcept; // TODO:
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept; // TODO:
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& fr) const noexcept; // TODO:
	[[nodiscard]] ContainmentType ContainedBy(const Plane& plane0, const Plane& plane1, const Plane& plane2, const Plane& Plane3, const Plane& Plane4, const Plane& Plane5) const noexcept;  // Test sphere against six planes (see BoundingFrustum::GetPlanes) // TODO:

	[[nodiscard]] bool Intersects(const Triangle& tri) const noexcept; // TODO:
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept; // TODO:
	[[nodiscard]] bool Intersects(const BoundingFrustum& fr) const noexcept; // TODO
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::optional<CollisionHit> Hit(const glm::vec3& point) const noexcept; // TODO
	[[nodiscard]] std::optional<CollisionHit> Hit(const Triangle& tri) const noexcept; // TODO
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingAABB& aabb) const noexcept; // TODO
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingOrientedBox& box) const noexcept; // TODO
	
	[[nodiscard]] static BoundingSphere CreateMerged(const BoundingSphere& S1, const BoundingSphere& S2) noexcept; // TODO:
	[[nodiscard]] static BoundingSphere CreateFromBoundingBox(const BoundingAABB& box) noexcept; // TODO:
	[[nodiscard]] static BoundingSphere CreateFromBoundingBox(const BoundingOrientedBox& box) noexcept; // TODO:
	[[nodiscard]] static BoundingSphere CreateFromPoints(const glm::vec3* points, std::size_t pointCount) noexcept; // TODO:
	[[nodiscard]] static BoundingSphere CreateFromFrustum(const BoundingFrustum& fr) noexcept; // TODO:

	glm::vec3 center = glm::vec3(0.0f);
	float radius = 0.0f;
};