#pragma once

#if USE_PHYSICS

class CollisionConvexObject3D
{
public:
	enum ObjectType 
	{
		Sphere = 0,
		Box,
		Capsule,
		Cylinder,
		Cone,
		ConvexHull,
		Triangle
	};

	// outerMargin Collision outer margin. Margin must be positive and will be added to the object to obtain his real size.
	explicit CollisionConvexObject3D(float outerMargin) : m_outerMargin(outerMargin) {}
	virtual ~CollisionConvexObject3D() = default;

	float GetOuterMargin() const { return m_outerMargin; }

	virtual CollisionConvexObject3D::ObjectType GetObjectType() const = 0;
	virtual glm::vec3 GetSupportPoint(const glm::vec3&, bool) const = 0;

	virtual std::string ToString() const = 0;

private:
	const float m_outerMargin;
};

class ObjectDeleter
{
public:
	void operator()(CollisionConvexObject3D*) const;
};

#endif // USE_PHYSICS