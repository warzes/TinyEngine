#pragma once

class OldBoundingFrustum; // TODO: delete

class Plane;
class Triangle;
class Ray;
class BoundingAABB;
class BoundingOrientedBox;
class BoundingFrustum;
class BoundingSphere;

// ContainmentType indicates whether a geometry object contains another one.
enum class ContainmentType : std::uint8_t
{
	Contains,
	Disjoint,
	Intersects
};

// PlaneIntersectionType indicates whether a geometry object intersects a plane.
enum class PlaneIntersectionType : std::uint8_t
{
	Front,
	Back,
	Intersecting,
};

struct CollisionHit
{
	glm::vec3 contactPoint;
	glm::vec3 normal;
	float depth;
};