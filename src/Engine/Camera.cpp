#include "stdafx.h"
#include "Camera.h"
//-----------------------------------------------------------------------------
void Camera::Teleport(const glm::vec3& pos)
{
	const glm::vec3 oldTarget = GetNormalizedViewVector();
	position = pos;
	target = pos + oldTarget;
}
//-----------------------------------------------------------------------------
void Camera::Teleport(const glm::vec3& pos, const glm::vec3& forwardLook)
{
	position = pos;
	target = pos + forwardLook;
}
//-----------------------------------------------------------------------------
void Camera::MoveBy(float distance)
{
	const glm::vec3 vOffset = GetNormalizedViewVector() * distance;
	position += vOffset;
	target += vOffset;
}
//-----------------------------------------------------------------------------
void Camera::StrafeBy(float distance)
{
	const glm::vec3 strafeVector = glm::normalize(glm::cross(GetNormalizedViewVector(), upVector)) * distance;
	position += strafeVector;
	target += strafeVector;
}
//-----------------------------------------------------------------------------
void Camera::RotateLeftRight(float angleInDegrees)
{
	const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::vec4 rotatedViewVector = rotationMatrix * glm::vec4(GetNormalizedViewVector(), 0.0f);
	target = position + glm::vec3(rotatedViewVector);
}
//-----------------------------------------------------------------------------
void Camera::RotateUpDown(float angleInDegrees)
{
	const glm::vec3 viewVector = GetNormalizedViewVector();
	const glm::vec3 viewVectorNoY = glm::normalize(glm::vec3(viewVector.x, 0.0f, viewVector.z));

	float currentAngleDegrees = glm::degrees(acos(glm::dot(viewVectorNoY, viewVector)));
	if (viewVector.y < 0.0f) currentAngleDegrees = -currentAngleDegrees;

	const float newAngleDegrees = currentAngleDegrees + angleInDegrees;
	if (newAngleDegrees > -85.0f && newAngleDegrees < 85.0f)
	{
		glm::vec3 rotationAxis = glm::cross(GetNormalizedViewVector(), upVector);
		rotationAxis = glm::normalize(rotationAxis);

		const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees), rotationAxis);
		const glm::vec4 rotatedViewVector = rotationMatrix * glm::vec4(GetNormalizedViewVector(), 0.0f);

		target = position + glm::vec3(rotatedViewVector);
	}
}
//-----------------------------------------------------------------------------
glm::vec3 Camera::GetNormalizedViewVector() const
{
	return glm::normalize(target - position);
}
//-----------------------------------------------------------------------------
glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(position, target, upVector);
}
//-----------------------------------------------------------------------------
glm::vec3 Camera::GetForwardVector() const
{
	return glm::normalize(glm::vec3(-target.x, 0.0f, -target.z));
}
//-----------------------------------------------------------------------------
glm::vec3 Camera::GetRightVector() const
{
	glm::vec3 right = glm::normalize(glm::rotateY(GetForwardVector(), -90.0f));
	return glm::vec3(right);
}
//-----------------------------------------------------------------------------
glm::vec3 Camera::GetUpVector() const
{
	return upVector;
}
//-----------------------------------------------------------------------------