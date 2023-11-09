#pragma once

#include "Core/Geometry/GeometryCore.h"

class BoundingOrientedBox final
{
public:
	static constexpr int CornerCount = 8;

	BoundingOrientedBox() noexcept = default;
	BoundingOrientedBox(BoundingOrientedBox&&) noexcept = default;
	BoundingOrientedBox(const BoundingOrientedBox&) noexcept = default;
	constexpr BoundingOrientedBox(const glm::vec3& Center, const glm::vec3& Extents, _In_ const glm::quat& Orientation) noexcept : center(Center), extents(Extents), orientation(Orientation) {}

	BoundingOrientedBox& operator=(BoundingOrientedBox&&) noexcept = default;
	BoundingOrientedBox& operator=(const BoundingOrientedBox&) noexcept = default;

	bool operator==(const BoundingOrientedBox& rhs) const noexcept { return (center == rhs.center && extents == rhs.extents && orientation == rhs.orientation); }
	bool operator!=(const BoundingOrientedBox& rhs) const noexcept { return (center != rhs.center || extents != rhs.extents || orientation != rhs.orientation); }

	void Transform(BoundingOrientedBox& Out, const glm::mat4& mat) const noexcept;
	void Transform(BoundingOrientedBox& Out, float scale, const glm::vec3& rotation, const glm::vec3& translation) const noexcept;

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const Triangle& tri) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& fr) const noexcept;
	[[nodiscard]] ContainmentType ContainedBy(const Plane& plane0, const Plane& plane1, const Plane& plane2, const Plane& Plane3, const Plane& Plane4, const Plane& Plane5) const noexcept; // Test OrientedBox against six planes (see BoundingFrustum::GetPlanes)

	[[nodiscard]] bool Intersects(const Triangle& tri) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingFrustum& fr) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::optional<CollisionHit> Hit(const glm::vec3& point) const noexcept;
	[[nodiscard]] std::optional<CollisionHit> Hit(const Triangle& tri) const noexcept;
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingOrientedBox& box) const noexcept;

	[[nodiscard]] std::array<glm::vec3, CornerCount> GetCorners() const noexcept; // Gets the 8 corners of the box

	[[nodiscard]] static BoundingAABB CreateFromBoundingBox(const BoundingAABB& box) noexcept;
	[[nodiscard]] static BoundingAABB CreateFromPoints(const glm::vec3* points, std::size_t pointCount) noexcept;

	glm::vec3 center = glm::vec3{ 0.0f };                      // Center of the box.
	glm::vec3 extents = glm::vec3{ 1.0f };                     // Distance from the center to each side.
	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Unit quaternion representing rotation (box -> world).
};