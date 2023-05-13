#pragma once

struct _012LightClass
{
	glm::vec4 diffuseColor;
	glm::vec3 position;

	void GetDiffuseColor(float* color)
	{
		color[0] = diffuseColor[0];
		color[1] = diffuseColor[1];
		color[2] = diffuseColor[2];
		color[3] = diffuseColor[3];
	}

	void GetPosition(float* outPosition)
	{
		outPosition[0] = position[0];
		outPosition[1] = position[1];
		outPosition[2] = position[2];
	}
};

class _012MultiplePointLights final : public IApp
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
	Uniform m_uniformLightPosition;
	Uniform m_uniformDiffuseColor;

	glm::mat4 m_perspective;

	GeometryBufferRef m_geom;
	Texture2DRef m_texture;

	unsigned m_numLights;
	int unused;
	_012LightClass* m_light = nullptr;

	Camera m_camera;

	float m_rotation = 360.0f;
};