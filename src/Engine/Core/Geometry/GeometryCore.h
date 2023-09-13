#pragma once

class Plane;
class BoundingAABB;
class BoundingFrustum;
class BoundingSphere;
class Ray;

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