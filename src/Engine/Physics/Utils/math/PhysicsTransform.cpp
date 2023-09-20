#include "stdafx.h"
#if USE_PHYSICS
#include "PhysicsTransform.h"
//-----------------------------------------------------------------------------
// TODO: взять из glm
glm::quat fromAxisAngle(const glm::vec3& normalizedAxis, float angle) 
{
	const float halfAngle = angle * 0.5f;
	const float sin = std::sin(halfAngle);
	return glm::quat(std::cos(halfAngle), normalizedAxis.x * sin, normalizedAxis.y * sin, normalizedAxis.z * sin);
}
//-----------------------------------------------------------------------------
PhysicsTransform::PhysicsTransform(const glm::vec3& position, const glm::quat& orientation)
	: m_position(position)
	, m_orientation(orientation)
{
}
//-----------------------------------------------------------------------------
void PhysicsTransform::SetPosition(const glm::vec3& position)
{
	m_position = position;
}
//-----------------------------------------------------------------------------
const glm::vec3& PhysicsTransform::GetPosition() const
{
	return m_position;
}
//-----------------------------------------------------------------------------
void PhysicsTransform::SetOrientation(const glm::quat& orientation)
{
	m_orientation = orientation;
}
//-----------------------------------------------------------------------------
const glm::quat& PhysicsTransform::GetOrientation() const
{
	return m_orientation;
}
//-----------------------------------------------------------------------------
glm::mat3 PhysicsTransform::RetrieveOrientationMatrix() const
{
	// TODO: есть ли аналог в glm?
	const float xx = m_orientation.x * m_orientation.x;
	const float xy = m_orientation.x * m_orientation.y;
	const float xz = m_orientation.x * m_orientation.z;
	const float xw = m_orientation.x * m_orientation.w;
	const float yy = m_orientation.y * m_orientation.y;
	const float yz = m_orientation.y * m_orientation.z;
	const float yw = m_orientation.y * m_orientation.w;
	const float zz = m_orientation.z * m_orientation.z;
	const float zw = m_orientation.z * m_orientation.w;

	// TODO: не нужно ли транспонировать
	return glm::mat3(
		1 - 2 * (yy + zz), 2 * (xy - zw), 2 * (xz + yw),
		2 * (xy + zw), 1 - 2 * (xx + zz), 2 * (yz - xw),
		2 * (xz - yw), 2 * (yz + xw), 1 - 2 * (xx + yy));
}
//-----------------------------------------------------------------------------
PhysicsTransform PhysicsTransform::Inverse() const
{
	const glm::quat invOrientation = glm::inverse(m_orientation);
	return PhysicsTransform(rotatePoint(invOrientation, -m_position), invOrientation);
}
//-----------------------------------------------------------------------------
bool PhysicsTransform::Equals(const PhysicsTransform& physicsTransform) const
{
	return physicsTransform.GetPosition() == m_position && physicsTransform.GetOrientation() == m_orientation;
}
//-----------------------------------------------------------------------------
glm::vec3 PhysicsTransform::Transform(const glm::vec3& point) const
{
	return rotatePoint(m_orientation, point) + m_position;
}
//-----------------------------------------------------------------------------
glm::vec3 PhysicsTransform::InverseTransform(const glm::vec3& point) const
{
	return rotatePoint(glm::conjugate(m_orientation), point - m_position);
}
//-----------------------------------------------------------------------------
PhysicsTransform PhysicsTransform::Integrate(const glm::vec3& linearVelocity, const glm::vec3& angularVelocity, float timeStep) const
{
	const glm::vec3 interpolatePosition = m_position + (linearVelocity * timeStep);
	glm::quat interpolateOrientation = m_orientation;

	const float length = angularVelocity.length();
	if (length > 0.0001) 
	{
		const glm::vec3  normalizedAxis = angularVelocity / length;
		const float angle = length * timeStep;

		interpolateOrientation = fromAxisAngle(normalizedAxis, angle) * interpolateOrientation;
		interpolateOrientation = glm::normalize(interpolateOrientation);
	}

	return PhysicsTransform(interpolatePosition, interpolateOrientation);
}
//-----------------------------------------------------------------------------
PhysicsTransform PhysicsTransform::operator*(const PhysicsTransform& transform) const
{
	return PhysicsTransform(m_position + rotatePoint(m_orientation, transform.GetPosition()), m_orientation * transform.GetOrientation());
}
//-----------------------------------------------------------------------------
const PhysicsTransform& PhysicsTransform::operator*=(const PhysicsTransform& transform)
{
	*this = *this * transform;
	return *this;
}
//-----------------------------------------------------------------------------
glm::vec3 PhysicsTransform::rotatePoint(const glm::quat& orientation, const glm::vec3& point) const
{
	const glm::quat finalResult = orientation * point * glm::conjugate(orientation);
	return { finalResult.x, finalResult.y, finalResult.z };
}
//-----------------------------------------------------------------------------
#endif USE_PHYSICS // USE_PHYSICS