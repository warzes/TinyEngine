#pragma once

#include "GeometryCore.h"

// BoundingBox is an axis-aligned bounding box in 3D space.
// Rename AABB???
class BoundingBox final
{
public:
	static constexpr int CornerCount = 8;

	BoundingBox() noexcept = default;
	BoundingBox(const BoundingBox&) noexcept = default;
	BoundingBox(const glm::vec3& min, const glm::vec3& max) noexcept : min(min), max(max) {}
	BoundingBox(float fmin, float fmax) noexcept : min(glm::vec3(fmin)), max(glm::vec3(fmax)) {}

#if USE_SSE
	BoundingBox(__m128 smin, __m128 smax) noexcept
	{
		_mm_storeu_ps(&min.x, smin);
		_mm_storeu_ps(&max.x, smax);
	}
#endif // USE_SSE

	BoundingBox& operator=(const BoundingBox& rhs) noexcept 
	{
		min = rhs.min;
		max = rhs.max;
		return *this;
	}

	bool operator==(const BoundingBox& rhs) const { return (min == rhs.min && max == rhs.max); }
	bool operator!=(const BoundingBox& rhs) const { return (min != rhs.min || max != rhs.max); }

	glm::vec3 GetCenter() const { return (max + min) * 0.5f; }
	glm::vec3 GetSize() const { return max - min; }
	glm::vec3 GetHalfSize() const { return (max - min) * 0.5f; }

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingBox& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] bool Intersects(const BoundingBox& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] std::array<glm::vec3, CornerCount> GetCorners() const noexcept;

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };

};