#pragma once

class Camera final
{
public:
	void Teleport(const glm::vec3& pos);
	void Teleport(const glm::vec3& pos, const glm::vec3& forwardLook);
	void MoveBy(float distance); // move front/back
	void StrafeBy(float distance); // move left/right
	void RotateLeftRight(float angleInDegrees); // rotate left/right
	void RotateUpDown(float angleInDegrees); // rotate up/down

	glm::vec3 GetNormalizedViewVector() const;
	glm::mat4 GetViewMatrix() const;

	glm::vec3 GetForwardVector() const; // TODO: проверить
	glm::vec3 GetRightVector() const; // TODO: проверить
	glm::vec3 GetUpVector() const; // TODO: проверить

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
};