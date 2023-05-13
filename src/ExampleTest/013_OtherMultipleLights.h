#pragma once

class _013OtherMultipleLights final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_lightingShader;
	GeometryBufferRef m_cube;
	Texture2DRef m_diffuseMap;
	Texture2DRef m_specularMap;

	Camera m_camera;
	glm::mat4 m_perspective;
	int unused;
};