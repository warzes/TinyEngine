#pragma once

#include "Plane.h"

class BoundingFrustum final
{
	static constexpr int CornerCount = 8;
	static constexpr int PlaneCount = 6;
public:
	BoundingFrustum() = default;
	BoundingFrustum(const glm::mat4& matrix);

	[[nodiscard]] const glm::mat4& GetMatrix() const noexcept;
	void SetMatrix(const glm::mat4& matrix) noexcept;

	[[nodiscard]] const Plane& GetNear() const noexcept;
	[[nodiscard]] const Plane& GetFar() const noexcept;
	[[nodiscard]] const Plane& GetLeft() const noexcept;
	[[nodiscard]] const Plane& GetRight() const noexcept;
	[[nodiscard]] const Plane& GetTop() const noexcept;
	[[nodiscard]] const Plane& GetBottom() const noexcept;
	[[nodiscard]] const std::array<glm::vec3, CornerCount>& GetCorners() const noexcept;

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] bool Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

private:
	void createPlanes();
	void createCorners();

	std::array<Plane, PlaneCount> m_planes;
	std::array<glm::vec3, CornerCount> m_corners;
	glm::mat4 m_matrix;
};