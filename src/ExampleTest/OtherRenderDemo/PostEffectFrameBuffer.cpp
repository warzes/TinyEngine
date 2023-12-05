#include "stdafx.h"
#include "PostEffectFrameBuffer.h"
#include "Common/DemoPlane.h"
#include "Common/DemoCube.h"
#include "Common/DemoQuad.h"
//-----------------------------------------------------------------------------
namespace
{
	ShaderProgramRef shaderPlane;
	GeometryBufferRef plane;
	glm::mat4 TransformPlane;

	ShaderProgramRef shaderCube;
	GeometryBufferRef cube;
	glm::mat4 TransformCube;

	ShaderProgramRef shaderQuad;
	GeometryBufferRef quad;
	glm::mat4 TransformQuad;

	Camera camera;

	Texture2DRef textureColorbuffer;
	RenderbufferRef rbo;
	FramebufferRef fbo;

	unsigned int effect_select = 0;
}
//-----------------------------------------------------------------------------
bool PostEffectFrameBuffer::Create()
{
	auto& renderSystem = GetRenderSystem();

	shaderPlane = CreateDemoPlaneShader();
	plane = CreateDemoPlane(shaderPlane);
	TransformPlane = glm::mat4(1.0f);
	TransformPlane = glm::scale(TransformPlane, { 2.0f, 1.0f, 2.0f });
	TransformPlane = glm::translate(TransformPlane, { 0.0f, -1.0f, 0.0f });

	shaderCube = CreateDemoCubeShader();
	cube = CreateDemoCube(shaderCube);
	TransformCube = glm::mat4(1.0f);

	shaderQuad = CreateDemoQuadShader();
	quad = CreateDemoQuad(shaderQuad);

	{
		Texture2DCreateInfo createInfo;
		createInfo.format = TexelsFormat::RGB_U8;
		createInfo.width = (uint16_t)GetWindowWidth();
		createInfo.height = (uint16_t)GetWindowHeight();
		Texture2DInfo info;
		info.magFilter = TextureMagFilter::Linear;
		info.minFilter = TextureMinFilter::Linear;
		info.mipmap = false;
		textureColorbuffer = renderSystem.CreateTexture2D(createInfo, info);
		rbo = renderSystem.CreateRenderbuffer({ GetWindowWidth(),  GetWindowHeight() }, ImageFormat::D24S8);
		fbo = renderSystem.CreateFramebuffer(textureColorbuffer, rbo);
	}

	camera.Teleport({ 3.0f, 2.0f, -4.0f });
	camera.target = glm::vec3(0.0f);
	GetInputSystem().SetMouseLock(true);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	glEnable(GL_MULTISAMPLE);

	return true;
}
//-----------------------------------------------------------------------------
void PostEffectFrameBuffer::Destroy()
{
	GetInputSystem().SetMouseLock(false);
	shaderPlane.reset();
	plane.reset();
	shaderCube.reset();
	cube.reset();
	shaderQuad.reset();
	quad.reset();

	textureColorbuffer.reset();
	rbo.reset();
	fbo.reset();
}
//-----------------------------------------------------------------------------
void PostEffectFrameBuffer::Render()
{
	auto& renderSystem = GetRenderSystem();

	//renderSystem.SetClearColor({ 0.0f, 0.64f, 0.91f });
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	//renderSystem.ClearFrame();

	renderSystem.Bind(fbo);
	renderSystem.SetClearColor({ 0.1f, 0.1f, 0.1f });
	renderSystem.ClearFrame();
	glEnable(GL_DEPTH_TEST);
	{
		renderSystem.Bind(shaderPlane);
		renderSystem.SetUniform("uProjection", m_perspective);
		renderSystem.SetUniform("uView", camera.GetViewMatrix());
		renderSystem.SetUniform("uWorld", TransformPlane);
		renderSystem.Draw(plane);

		renderSystem.Bind(shaderCube);
		renderSystem.SetUniform("uProjection", m_perspective);
		renderSystem.SetUniform("uView", camera.GetViewMatrix());
		renderSystem.SetUniform("uWorld", TransformCube);
		renderSystem.Draw(cube);
	}
	renderSystem.ResetState(ResourceType::Framebuffer);
	renderSystem.SetClearColor({ 0.1f, 0.1f, 0.7f });
	renderSystem.ClearFrame();
	glDisable(GL_DEPTH_TEST);
	// draw quad
	{
		renderSystem.Bind(shaderQuad);
		renderSystem.Bind(fbo->colorTextures[0], 0);
		renderSystem.SetUniform("effect_select", effect_select);
		renderSystem.SetUniform("tex_width", (float)GetWindowWidth());
		renderSystem.SetUniform("tex_height", (float)GetWindowHeight());
		renderSystem.SetUniform("tex", 0);
		renderSystem.Draw(quad);
	}

}
//-----------------------------------------------------------------------------
void PostEffectFrameBuffer::Update(float deltaTime)
{
	auto& input = GetInputSystem();
	if (input.IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}

	if (input.IsKeyDown(Input::KEY_0))
		effect_select = 0; // normal
	if (input.IsKeyDown(Input::KEY_1))
		effect_select = 1; // inverted colours
	if (input.IsKeyDown(Input::KEY_2))
		effect_select = 2; // small gaussian blur
	if (input.IsKeyDown(Input::KEY_3))
		effect_select = 3; // medium gaussian blur
	if (input.IsKeyDown(Input::KEY_4))
		effect_select = 4; // big gaussian blur
	if (input.IsKeyDown(Input::KEY_5))
		effect_select = 5; // edge gaussian blur
	if (input.IsKeyDown(Input::KEY_6))
		effect_select = 6; // broken glass

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 100.f);

	const float mouseSensitivity = 10.0f * deltaTime;
	const float moveSpeed = 10.0f * deltaTime;
	const glm::vec3 oldCameraPos = camera.position;

	if (input.IsKeyDown(Input::KEY_W)) camera.MoveBy(moveSpeed);
	if (input.IsKeyDown(Input::KEY_S)) camera.MoveBy(-moveSpeed);
	if (input.IsKeyDown(Input::KEY_A)) camera.StrafeBy(moveSpeed);
	if (input.IsKeyDown(Input::KEY_D)) camera.StrafeBy(-moveSpeed);

	glm::vec2 delta = input.GetMouseDeltaPosition();
	if (delta.x != 0.0f) camera.RotateLeftRight(delta.x * mouseSensitivity);
	if (delta.y != 0.0f) camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------