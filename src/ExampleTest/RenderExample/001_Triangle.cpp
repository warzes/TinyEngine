#include "stdafx.h"
#include "001_Triangle.h"
//-----------------------------------------------------------------------------
namespace
{
	const char* vertexShaderText = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 ProjectionMatrix;

out vec3 Color;

void main()
{
	gl_Position = ProjectionMatrix * vec4(aPosition, 1.0);
	Color       = aColor;
}
)";

	const char* fragmentShaderText = R"(
in vec3 Color;
out vec4 FragmentColor;

void main()
{
	FragmentColor = vec4(Color, 1.0);
}
)";

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	} vert[] =
	{
		{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 1.0f, -1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.0f,  1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}}
	};
}
//-----------------------------------------------------------------------------
bool _001Triangle::Create()
{
	glEnable(GL_CULL_FACE); // для теста - треугольник выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "ProjectionMatrix");
	
	m_vb = renderSystem.CreateVertexBuffer(BufferUsage::StaticDraw, static_cast<unsigned>(Countof(vert)), static_cast<unsigned>(sizeof(Vertex)), vert);
	m_vao = renderSystem.CreateVertexArray(m_vb, nullptr, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _001Triangle::Destroy()
{
	m_shader.reset();
	m_vb.reset();
	m_vao.reset();
}
//-----------------------------------------------------------------------------
void _001Triangle::Render()
{
	auto& renderSystem = GetRenderSystem();

	renderSystem.SetClearColor({0.0f, 0.64f, 0.91f});
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	renderSystem.ClearFrame();

	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.Draw(m_vao);
}
//-----------------------------------------------------------------------------
void _001Triangle::Update(float /*deltaTime*/)
{
	if( GetInputSystem().IsKeyDown(Input::KEY_ESCAPE) )
	{
		ExitRequest();
		return;
	}

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 100.f);
}
//-----------------------------------------------------------------------------