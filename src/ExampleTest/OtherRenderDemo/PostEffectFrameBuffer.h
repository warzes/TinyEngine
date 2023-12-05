#pragma once

class PostEffectFrameBuffer final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	glm::mat4 m_perspective;
};