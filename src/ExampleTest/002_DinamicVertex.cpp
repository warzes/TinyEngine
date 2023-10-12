#include "stdafx.h"
#include "002_DinamicVertex.h"
//-----------------------------------------------------------------------------
struct testVertex
{
	glm::vec3 pos;
	glm::vec3 color;
};
//-----------------------------------------------------------------------------
static float pos = 0.0f;
static bool invert = false;
//-----------------------------------------------------------------------------
bool _002DinamicVertex::Create()
{
	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 projectionMatrix;

out vec3 fragmentColor;

void main()
{
	gl_Position   = projectionMatrix * vec4(position, 1.0);
	fragmentColor = color;
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

in vec3 fragmentColor;
out vec4 color;

void main()
{
	color = vec4(fragmentColor, 1.0);
}
)";

	glEnable(GL_CULL_FACE); // для теста - треугольник выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");
	m_vb = renderSystem.CreateVertexBuffer(BufferUsage::DynamicDraw);
	m_vao = renderSystem.CreateVertexArray(m_vb, nullptr, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _002DinamicVertex::Destroy()
{
	m_shader.reset();
	m_vb.reset();
	m_vao.reset();
}
//-----------------------------------------------------------------------------
void _002DinamicVertex::Render()
{
	auto& renderSystem = GetRenderSystem();

	if (m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight())
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	{
		testVertex vert[] =
		{
			{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}},
			{{ pos,   1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}}
		};
		renderSystem.UpdateBuffer(m_vb, 0, (unsigned)Countof(vert), (unsigned)sizeof(testVertex), vert);
	}

	renderSystem.ClearFrame();
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.Draw(m_vao);
}
//-----------------------------------------------------------------------------
void _002DinamicVertex::Update(float deltaTime)
{
	pos += deltaTime * (invert ? -1.0f : 1.0f);

	if (pos < -1.0f || pos > 1.0f)
		invert = !invert;

	if (GetInputSystem().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------