#pragma once

#include "Core/Geometry/Plane.h"

class BoundingFrustum final
{
public:
	static constexpr int CornerCount = 8;

	BoundingFrustum() = default;
	BoundingFrustum(BoundingFrustum&&) = default;
	BoundingFrustum(const BoundingFrustum&) = default;
	constexpr BoundingFrustum(const glm::vec3& Origin, const glm::quat& Orientation, float RightSlope, float LeftSlope, float TopSlope, float BottomSlope, float NearPlane, float FarPlane) noexcept : origin(Origin), orientation(Orientation), rightSlope(RightSlope), leftSlope(LeftSlope), topSlope(TopSlope), bottomSlope(BottomSlope), near(NearPlane), far(FarPlane) {}
	BoundingFrustum(const glm::mat4& projection, bool rhcoords = false) noexcept;

	BoundingFrustum& operator=(BoundingFrustum&&) = default;
	BoundingFrustum& operator=(const BoundingFrustum&) = default;

	void Transform(BoundingFrustum& Out, const glm::mat4& mat) const noexcept;
	void Transform(BoundingFrustum& Out, float scale, const glm::vec3& Rotation, const glm::vec3& Translation) const noexcept;

	[[nodiscard]] ContainmentType Contains(const glm::vec3& point) const noexcept;
	[[nodiscard]] ContainmentType Contains(const Triangle& tri) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] ContainmentType ContainedBy(const Plane& plane0, const Plane& plane1, const Plane& plane2, const Plane& Plane3, const Plane& Plane4, const Plane& Plane5) const noexcept; // Test frustum against six planes (see BoundingFrustum::GetPlanes)

	[[nodiscard]] bool Intersects(const Triangle& tri) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingAABB& aabb) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept;
	[[nodiscard]] bool Intersects(const BoundingFrustum& frustum) const noexcept;
	[[nodiscard]] PlaneIntersectionType Intersects(const Plane& plane) const noexcept;
	[[nodiscard]] std::optional<float> Intersects(const Ray& ray) const noexcept;

	[[nodiscard]] const std::array<glm::vec3, CornerCount>& GetCorners() const noexcept;

	void GetPlanes(glm::vec4* NearPlane, glm::vec4* FarPlane, glm::vec4* RightPlane, glm::vec4* LeftPlane, glm::vec4* TopPlane, glm::vec4* BottomPlane) const noexcept; // Create 6 Planes representation of Frustum

	static void CreateFromMatrix(BoundingFrustum& Out, const glm::mat4& projection, bool rhcoords = false) noexcept;

	//[[nodiscard]] const glm::mat4& GetMatrix() const noexcept;
	//void SetMatrix(const glm::mat4& matrix) noexcept;

	//[[nodiscard]] const Plane& GetNear() const noexcept;
	//[[nodiscard]] const Plane& GetFar() const noexcept;
	//[[nodiscard]] const Plane& GetLeft() const noexcept;
	//[[nodiscard]] const Plane& GetRight() const noexcept;
	//[[nodiscard]] const Plane& GetTop() const noexcept;
	//[[nodiscard]] const Plane& GetBottom() const noexcept;


	glm::vec3 origin = glm::vec3(0.0f);                        // Origin of the frustum (and projection).
	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Quaternion representing rotation.

	float rightSlope = 1.0f;                                   // Positive X (X/Z)
	float leftSlope = -1.0f;                                   // Negative X
	float topSlope = 1.0f;                                     // Positive Y (Y/Z)
	float bottomSlope = -1.0f;                                 // Negative Y
	float near = 0.0f, far = 1.0f;                             // Z of the near plane and far plane.
};

class OldBoundingFrustum final
{
	static constexpr int CornerCount = 8;
	static constexpr int PlaneCount = 6;
public:
	OldBoundingFrustum() = default;
	OldBoundingFrustum(const glm::mat4& matrix);

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
	[[nodiscard]] ContainmentType Contains(const OldBoundingFrustum& frustum) const noexcept;
	[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

	[[nodiscard]] bool Intersects(const BoundingAABB& box) const noexcept;
	[[nodiscard]] bool Intersects(const OldBoundingFrustum& frustum) const noexcept;
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