#pragma once

#include "GeometryCore.h"

// BoundingAABB is an Axis-Aligned Bounding-Box in 3D space.
class BoundingAABB final
{
public:
	static constexpr int CornerCount = 8;

	BoundingAABB() noexcept = default;
	BoundingAABB(const BoundingAABB&) noexcept = default;
	BoundingAABB(const glm::vec3& _min, const glm::vec3& _max) noexcept : min(_min), max(_max) {}
	BoundingAABB(float fmin, float fmax) noexcept : min(glm::vec3(fmin)), max(glm::vec3(fmax)) {}

	BoundingAABB& operator=(const BoundingAABB&) noexcept = default;

	bool operator==(const BoundingAABB& rhs) const noexcept { return (min == rhs.min && max == rhs.max); }
	bool operator!=(const BoundingAABB& rhs) const noexcept { return (min != rhs.min || max != rhs.max); }

	void Insert(const glm::vec3& point) noexcept;
	void Insert(const BoundingAABB& otherAABB) noexcept;

	void Translate(const glm::vec3& position) noexcept;

	[[nodiscard]] glm::vec3 GetCenter() const noexcept { return (max + min) * 0.5f; }
	[[nodiscard]] glm::vec3 GetSize() const noexcept { return max - min; }
	[[nodiscard]] glm::vec3 GetHalfSize() const noexcept { return (max - min) * 0.5f; }

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] bool Intersects(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::array<glm::vec3, CornerCount> GetCorners() const noexcept;

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };
};

namespace StringUtils
{
	bool FromString(BoundingAABB& out, const char* string);
	std::string ToString(const BoundingAABB& in);
}