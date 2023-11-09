#pragma once

#include "Core/Geometry/GeometryCore.h"

class Triangle final
{
public:
	Triangle() = default;
	Triangle(Triangle&&) noexcept = default;
	Triangle(const Triangle&) noexcept = default;

	Triangle& operator=(Triangle&&) noexcept = default;
	Triangle& operator=(const Triangle&) noexcept = default;

	bool operator==(const Triangle& rhs) const noexcept { return (p0 == rhs.p0 && p1 == rhs.p1 && p2 == rhs.p2); }
	bool operator!=(const Triangle& rhs) const noexcept { return (p0 != rhs.p0 || p1 != rhs.p1 || p2 != rhs.p2); }

	[[nodiscard]] bool Intersects(const Triangle& tri) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] ContainmentType ContainedBy(const Plane& plane0, const Plane& plane1, const Plane& plane2, const Plane& Plane3, const Plane& Plane4, const Plane& Plane5) const noexcept; // Test a triangle against six planes at once (see BoundingFrustum::GetPlanes)

	glm::vec3 p0 = glm::vec3(0.0f);
	glm::vec3 p1 = glm::vec3(0.0f);
	glm::vec3 p2 = glm::vec3(0.0f);
};