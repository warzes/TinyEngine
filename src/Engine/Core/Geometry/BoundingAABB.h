#pragma once

#include "GeometryCore.h"

// BoundingBox is an axis-aligned bounding box in 3D space.
class BoundingAABB final
{
public:
	static constexpr int CornerCount = 8;

	BoundingAABB() noexcept = default;
	BoundingAABB(const BoundingAABB&) noexcept = default;
	BoundingAABB(const glm::vec3& _min, const glm::vec3& _max) noexcept : min(_min), max(_max) {}
	BoundingAABB(float fmin, float fmax) noexcept : min(glm::vec3(fmin)), max(glm::vec3(fmax)) {}

	BoundingAABB& operator=(const BoundingAABB&) noexcept = default;

	bool operator==(const BoundingAABB& rhs) const { return (min == rhs.min && max == rhs.max); }
	bool operator!=(const BoundingAABB& rhs) const { return (min != rhs.min || max != rhs.max); }

	void AddPoint(const glm::vec3& point);
	void Merge(const BoundingAABB& rhs);
	// expands the volume to include v
	void Include(const glm::vec3& v) { AddPoint(v); }
	void Include(const BoundingAABB& aabb) { Merge(aabb); }

	void Translate(const glm::vec3& v);

	glm::vec3 GetCenter() const { return (max + min) * 0.5f; }
	glm::vec3 GetSize() const { return max - min; }
	glm::vec3 GetHalfSize() const { return (max - min) * 0.5f; }

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] bool Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::array<glm::vec3, CornerCount> GetCorners() const noexcept;

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };
};