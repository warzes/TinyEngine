#pragma once

class AABB
{
public:
	AABB() = default;
	AABB(const AABB&) = default;
	AABB(const glm::vec3& _min, const glm::vec3& _max) : min(_min), max(_max) {}

	AABB& operator=(const AABB&) = default;

	// expands the volume to include v
	void Include(const glm::vec3& v)
	{
		min = glm::min(min, v);
		max = glm::max(max, v);
	}

	// expands the volume to include v
	void Include(const AABB& aabb)
	{
		min = glm::min(min, aabb.min);
		max = glm::max(max, aabb.max);
	}

	// returns the dimensions (width/height/depth) of this axis aligned box.
	glm::vec3 GetDimensions() const { return max - min; }
	glm::vec3 GetExtents() const { return GetDimensions() * 0.5f; }
	glm::vec3 GetCenter() const { return (min + max) * 0.5f; }

	// Finds the closest point in the box to the point p. If p is contained, this will be p, otherwise it will be the closest point on the surface of the box.
	glm::vec3 ClosestPoint(const glm::vec3& p) const { return glm::max(min, glm::min(max, p)); }

	// checks a box is inside another box.
	bool IsInside(const AABB& aabb) const
	{
		if( aabb.min.x > min.x ) return false;
		if( aabb.min.y > min.y ) return false;
		if( aabb.min.z > min.z ) return false;
		if( aabb.max.x < max.x ) return false;
		if( aabb.max.y < max.y ) return false;
		if( aabb.max.z < max.z ) return false;
		return true;
	}

	// indicates if these bounds contain v.
	bool Contains(const glm::vec3& v) const
	{
		return !(v.x < min.x || v.x > max.x || v.y < min.y || v.y > max.y || v.z < min.z || v.z > max.z);
	}

	// indicates whether the intersection of this and b is empty or not.
	bool Intersects(const AABB& aabb) const
	{
		return !(aabb.min.x > max.x || min.x > aabb.max.x || aabb.min.y > max.y || min.y > aabb.max.y || aabb.min.z > max.z || min.z > aabb.max.z);
	}

	// computes the 1D-intersection between two AABBs, on a given axis.
	bool Intersects1D(const AABB& aabb, uint32_t axis) const
	{
		return max[axis] >= aabb.min[axis] && aabb.max[axis] >= min[axis];
	}

	glm::vec3 min = glm::vec3{ std::numeric_limits<float>::max() };
	glm::vec3 max = glm::vec3{ std::numeric_limits<float>::lowest() };
};