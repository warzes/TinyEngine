#pragma once

#if USE_PHYSICS

// Class allow to create transform matrix from a given position and orientation

class PhysicsTransform final
{
public:
	PhysicsTransform() = default;
	explicit PhysicsTransform(const glm::vec3& position, const glm::quat& orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

	void SetPosition(const glm::vec3& position);
	const glm::vec3& GetPosition() const;

	void SetOrientation(const glm::quat& orientation);
	const glm::quat& GetOrientation() const;
	glm::mat3 RetrieveOrientationMatrix() const;

	PhysicsTransform Inverse() const;

	bool Equals(const PhysicsTransform& physicsTransform) const;
	glm::vec3 Transform(const glm::vec3& point) const;
	glm::vec3 InverseTransform(const glm::vec3& point) const;
	PhysicsTransform Integrate(const glm::vec3& linearVelocity, const glm::vec3& angularVelocity, float timeStep) const;

	PhysicsTransform operator*(const PhysicsTransform& transform) const;
	const PhysicsTransform& operator*=(const PhysicsTransform& transform);

private:
	glm::vec3 rotatePoint(const glm::quat& orientation, const glm::vec3& point) const;

	glm::vec3 m_position = glm::vec3(0.0f);
	glm::quat m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

#endif USE_PHYSICS // USE_PHYSICS