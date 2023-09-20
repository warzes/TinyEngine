#pragma once

#if USE_PHYSICS

#include "Physics/Object/CollisionConvexObject.h"

class CollisionBoxObject final : public CollisionConvexObject3D 
{
public:
	CollisionBoxObject(float, glm::vec3&, const glm::vec3&, const glm::quat&);

	float GetHalfSize(unsigned int) const;
	glm::vec3 GetHalfSizes() const;
	const glm::vec3& GetCenterOfMass() const;
	const glm::quat& GetOrientation() const;
	const glm::vec3& GetNormalizedAxis(unsigned int) const;

	CollisionConvexObject3D::ObjectType GetObjectType() const override;
	glm::vec3 GetSupportPoint(const glm::vec3&, bool) const override;

	OBBox<float> RetrieveOBBox() const;
	std::string ToString() const override;

private:
	const OBBox<float> m_boxObject; //object without margin
};

#endif // USE_PHYSICS