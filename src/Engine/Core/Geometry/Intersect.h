#pragma once

class Plane;

namespace Intersect
{
	// Compute barycentric coordinates (u,v,w) for a point P with respect to triangle (a,b,c).
	inline glm::vec3 Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p);

	// Test if point p lies inside the triangle (a,b,c).
	inline bool TestPointTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
	// Test if point p lies inside the counter clock wise triangle (a,b,c).
	inline bool TestPointTriangleCCW(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

	// Find the closest point to 'p' on a triangle (a,b,c).
	inline glm::vec3 ClosestPointTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

	// This method checks if a point is inside a sphere or outside it.
	inline bool PointSphere(const glm::vec3& centerPosition, float radiusSquared, const glm::vec3& testPoint);

	// This method checks if a point is inside a cylinder or outside it.
	inline bool PointCylinder(const glm::vec3& baseCenterPoint, const glm::vec3& axisVector, float axisLengthSquared, float radiusSquared, const glm::vec3& testPoint);
}

#include "Core/Geometry/Intersect.inl"