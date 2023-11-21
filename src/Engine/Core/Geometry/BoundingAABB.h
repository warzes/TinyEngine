#pragma once

#include "Core/Geometry/GeometryCore.h"

// BoundingAABB is an Axis-Aligned Bounding-Box in 3D space.
class BoundingAABB final
{
public:
	static constexpr int CornerCount = 8;

	BoundingAABB() noexcept = default;
	BoundingAABB(BoundingAABB&&) noexcept = default;
	BoundingAABB(const BoundingAABB&) noexcept = default;
	BoundingAABB(const glm::vec3& _min, const glm::vec3& _max) noexcept : min(_min), max(_max) {}
	BoundingAABB(float fmin, float fmax) noexcept : min(glm::vec3(fmin)), max(glm::vec3(fmax)) {}

	BoundingAABB& operator=(BoundingAABB&&) noexcept = default;
	BoundingAABB& operator=(const BoundingAABB&) noexcept = default;

	bool operator==(const BoundingAABB& rhs) const noexcept { return (min == rhs.min && max == rhs.max); }
	bool operator!=(const BoundingAABB& rhs) const noexcept { return (min != rhs.min || max != rhs.max); }

	void Transform(BoundingAABB& Out, const glm::mat4& mat) const noexcept;
	void Transform(BoundingAABB& Out, float scale, const glm::vec3& rotation, const glm::vec3& translation) const noexcept;

	void Insert(const glm::vec3& point) noexcept;
	void Insert(const BoundingAABB& otherAABB) noexcept;

	void Translate(const glm::vec3& position) noexcept;

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const Triangle& tri) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& fr) const noexcept;
	[[nodiscard]] ContainmentType ContainedBy(const Plane& plane0, const Plane& plane1, const Plane& plane2, const Plane& Plane3, const Plane& Plane4, const Plane& Plane5) const noexcept; // Test box against six planes (see BoundingFrustum::GetPlanes)

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

	[[nodiscard]] glm::vec3 GetCenter() const noexcept { return (max + min) * 0.5f; } // TODO: проверить, возможно нужно так (min + (max-min)*0.5)
	[[nodiscard]] glm::vec3 GetSize() const noexcept { return max - min; }
	[[nodiscard]] glm::vec3 GetHalfSize() const noexcept { return (max - min) * 0.5f; }

	[[nodiscard]] static BoundingAABB CreateFromCenterAndHalfExtents(const glm::vec3& center, const glm::vec3& halfExtents) noexcept;
	[[nodiscard]] static BoundingAABB CreateMerged(const BoundingAABB& b1, const BoundingAABB& b2) noexcept;
	[[nodiscard]] static BoundingAABB CreateFromSphere(const BoundingSphere& sphere) noexcept;
	[[nodiscard]] static BoundingAABB CreateFromPoints(const glm::vec3& pt1, const glm::vec3& pt2) noexcept;
	[[nodiscard]] static BoundingAABB CreateFromPoints(const glm::vec3* points, std::size_t pointCount) noexcept;

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };
};

namespace StringUtils
{
	bool FromString(BoundingAABB& out, const char* string);
	std::string ToString(const BoundingAABB& in);
}