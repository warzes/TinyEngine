#pragma once

class _008AlphaMapping final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	glm::mat4 m_perspective;
	GeometryBufferRef m_geom;
	Texture2DRef m_texture1;
	Texture2DRef m_texture2;
	Texture2DRef m_texture3;
};