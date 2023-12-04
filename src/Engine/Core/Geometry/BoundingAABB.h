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
	BoundingAABB(const glm::vec3* points, size_t numPoints) noexcept;
	BoundingAABB(const std::vector<glm::vec3> points) noexcept;

	BoundingAABB& operator=(BoundingAABB&&) noexcept = default;
	BoundingAABB& operator=(const BoundingAABB&) noexcept = default;

	bool operator==(const BoundingAABB& rhs) const noexcept { return (min == rhs.min && max == rhs.max); }
	bool operator!=(const BoundingAABB& rhs) const noexcept { return (min != rhs.min || max != rhs.max); }

	void Set(const glm::vec3& min, const glm::vec3& max) noexcept;
	void Set(float fmin, float fmax) noexcept;
	void Set(const glm::vec3* points, size_t numPoints) noexcept;
	void Set(const glm::vec3* points, size_t numPoints, const glm::mat4& transform) noexcept;
	void SetFromTransformedAABB(const BoundingAABB& aabb, const glm::mat4& transform) noexcept;

	void Merge(const BoundingAABB& other) noexcept;
	void Merge(const glm::vec3& point) noexcept;
	void Merge(const BoundingAABB& other, const glm::mat4& transform) noexcept;
	void Merge(const glm::vec3& point, const glm::mat4& transform) noexcept;
	void Merge(const BoundingAABB& other, const glm::mat3& transform) noexcept;
	void Merge(const glm::vec3& point, const glm::mat3& transform) noexcept;

	void Translate(const glm::vec3& position) noexcept;
	void Scale(const glm::vec3& scale) noexcept;
	void Rotate(const glm::mat3& rotation) noexcept;

	void Transform(const glm::mat4& transform) noexcept;
	void Transform(float scale, const glm::vec3& rotation, const glm::vec3& translation) noexcept; // TODO: нереализовано
	BoundingAABB Transformed(const glm::mat4& transform) const noexcept;

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const Triangle& tri) const noexcept; // TODO: нереализовано
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept; // TODO: нереализовано
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& fr) const noexcept; // TODO: нереализовано

	[[nodiscard]] bool Intersects(const glm::vec3& point) const noexcept;
	[[nodiscard]] bool Intersects(const glm::vec3& point0, const glm::vec3& point1) const noexcept; // IntersectSegment
	[[nodiscard]] bool Intersects(const Triangle& tri) const noexcept; // TODO: нереализовано
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept; // TODO: нереализовано
	[[nodiscard]] bool Intersects(const BoundingFrustum& fr) const noexcept; // TODO: нереализовано
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::optional<CollisionHit> Hit(const glm::vec3& point) const noexcept; // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const Triangle& tri) const noexcept; // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingSphere& sphere) const noexcept; // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingAABB& aabb) const noexcept; // TODO: нереализовано
	[[nodiscard]] std::optional<CollisionHit> Hit(const BoundingOrientedBox& box) const noexcept; // TODO: нереализовано

	[[nodiscard]] std::array<glm::vec3, CornerCount> GetCorners() const noexcept; // Gets the 8 corners of the box

	[[nodiscard]] glm::vec3 GetCenter() const noexcept { return (max + min) * 0.5f; }
	[[nodiscard]] glm::vec3 GetSize() const noexcept { return max - min; }
	[[nodiscard]] glm::vec3 GetHalfSize() const noexcept { return GetSize() * 0.5f; }

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };
};

namespace StringUtils
{
	bool FromString(BoundingAABB& out, const char* string);
	std::string ToString(const BoundingAABB& in);
}