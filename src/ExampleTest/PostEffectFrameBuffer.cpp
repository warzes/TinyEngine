#include "stdafx.h"
#include "PostEffectFrameBuffer.h"
//-----------------------------------------------------------------------------
namespace // shader plane
{
	constexpr const char* vsPlane = R"(
layout(location = 0) in vec3 aPosition;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out float Red;
out float Green;
out float Blue;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(aPosition, 1.0);
	Red = Green = Blue = 0.0;
	if(gl_VertexID == 0){
		Red = 1.0;
	}
	else if(gl_VertexID == 1){
		Green = 1.0;
	}
	else if(gl_VertexID == 2){
		Blue = 1.0;
	}
}
)";

	constexpr const char* fsPlane = R"(
in float Red;
in float Green;
in float Blue;

out vec4 FragmentColor;

void main()
{
	FragmentColor = vec4(Red, Green, Blue, 1.0);
}
)";
}
//-----------------------------------------------------------------------------
namespace
{
	ShaderProgramRef shaderPlane;
	GeometryBufferRef plane;

	Camera camera;
}
//-----------------------------------------------------------------------------
bool PostEffectFrameBuffer::Create()
{
	auto& renderSystem = GetRenderSystem();

	shaderPlane = renderSystem.CreateShaderProgram({ vsPlane }, { fsPlane });
	{
		glm::vec3 vpoint[] =
		{
			{-1.0f, 0.0f,  1.0f },
			{ 1.0f, 0.0f,  1.0f },
			{ 1.0f, 0.0f, -1.0f },
			{-1.0f, 0.0f, -1.0f }
		};

		unsigned vpoint_index[] = {
			0, 1, 2,
			0, 2, 3
		};

		plane = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw,
			static_cast<unsigned>(Countof(vpoint)),
			static_cast<unsigned>(sizeof(glm::vec3)), 
			vpoint, 
			static_cast<unsigned>(Countof(vpoint_index)),
			IndexFormat::UInt32,
			vpoint_index,
			shaderPlane);
	}


	camera.Teleport({ 0.0f, 0.2f, -3.0f });
	GetInputSystem().SetMouseLock(true);

	return true;
}
//-----------------------------------------------------------------------------
void PostEffectFrameBuffer::Destroy()
{
	GetInputSystem().SetMouseLock(false);
	shaderPlane.reset();
	plane.reset();
}
//-----------------------------------------------------------------------------
void PostEffectFrameBuffer::Render()
{
	auto& renderSystem = GetRenderSystem();

	renderSystem.SetClearColor({ 0.0f, 0.64f, 0.91f });
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	renderSystem.ClearFrame();

	renderSystem.Bind(shaderPlane);
	renderSystem.SetUniform("uProjection", m_perspective);
	renderSystem.SetUniform("uView", camera.GetViewMatrix());
	renderSystem.SetUniform("uWorld", glm::mat4(1.0f));

	renderSystem.Draw(plane);
}
//-----------------------------------------------------------------------------
void PostEffectFrameBuffer::Update(float deltaTime)
{
	if (GetInputSystem().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 100.f);

	const float mouseSensitivity = 10.0f * deltaTime;
	const float moveSpeed = 10.0f * deltaTime;
	const glm::vec3 oldCameraPos = camera.position;

	if (GetInputSystem().IsKeyDown(Input::KEY_W)) camera.MoveBy(moveSpeed);
	if (GetInputSystem().IsKeyDown(Input::KEY_S)) camera.MoveBy(-moveSpeed);
	if (GetInputSystem().IsKeyDown(Input::KEY_A)) camera.StrafeBy(moveSpeed);
	if (GetInputSystem().IsKeyDown(Input::KEY_D)) camera.StrafeBy(-moveSpeed);

	glm::vec2 delta = GetInputSystem().GetMouseDeltaPosition();
	if (delta.x != 0.0f) camera.RotateLeftRight(delta.x * mouseSensitivity);
	if (delta.y != 0.0f) camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------