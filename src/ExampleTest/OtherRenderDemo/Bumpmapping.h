#pragma once

class Bumpmapping final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	glm::mat4 m_perspective;
};