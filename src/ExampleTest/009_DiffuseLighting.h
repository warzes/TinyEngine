#pragma once

struct _009LightClass
{
	glm::vec4 diffuseColor;
	glm::vec3 direction;
};

class _009DiffuseLighting final : public IApp
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
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;
	Uniform m_uniformLightDirection;
	Uniform m_uniformLightColor;

	glm::mat4 m_perspective;

	GeometryBufferRef m_geom;
	Texture2DRef m_texture;

	_009LightClass m_diffuse;

	float m_rotation = 360.0f;

	Camera m_camera;

	int unused;
};