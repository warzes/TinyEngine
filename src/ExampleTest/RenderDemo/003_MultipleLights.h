#pragma once

class _003MultipleLights final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	ShaderProgramRef m_shaderObject;
	Uniform m_uniformObjectProjectionMatrix;
	Uniform m_uniformObjectViewMatrix;
	Uniform m_uniformObjectWorldMatrix;
	Uniform m_uniformObjectViewPos;

	ShaderProgramRef m_shaderLightSource;
	Uniform m_uniformLightSourceProjectionMatrix;
	Uniform m_uniformLightSourceViewMatrix;
	Uniform m_uniformLightSourceWorldMatrix;

	glm::mat4 m_perspective;
	Camera m_camera;
	GeometryBufferRef m_geom;

	Texture2DRef m_diffuse;
	Texture2DRef m_specular;
};