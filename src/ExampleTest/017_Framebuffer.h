#pragma once

class _017Framebuffer final : public IApp
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

	ShaderProgramRef screenShader;

	GeometryBufferRef m_geomBox;
	GeometryBufferRef m_geomPlane;
	GeometryBufferRef m_geomQuad;

	Texture2DRef cubeTexture;
	Texture2DRef floorTexture;

	Texture2DRef m_textureColorbuffer;
	RenderbufferRef m_rbo;
	FramebufferRef m_fbo;

	glm::mat4 m_perspective;
	Camera m_camera;
	int unused;
};