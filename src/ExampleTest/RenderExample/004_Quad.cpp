#include "stdafx.h"
#include "004_Quad.h"
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
		{{ -0.5f,  0.5f, 2.0f}, {0.0f, 1.0f, 1.0f}}, // top left
		{{  0.5f,  0.5f, 2.0f}, {1.0f, 0.0f, 0.0f}}, // top right
		{{  0.5f, -0.5f, 2.0f}, {0.0f, 1.0f, 0.0f}}, // bottom right
		{{ -0.5f, -0.5f, 2.0f}, {0.0f, 0.0f, 1.0f}}, // bottom left
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 2,   // first triangle
		2, 1, 0    // second triangle
	};
}
//-----------------------------------------------------------------------------
bool _004Quad::Create()
{
	glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "ProjectionMatrix");

	m_vb = renderSystem.CreateVertexBuffer(BufferUsage::StaticDraw, static_cast<unsigned>(Countof(vert)), static_cast<unsigned>(sizeof(Vertex)), vert);
	m_ib = renderSystem.CreateIndexBuffer(BufferUsage::StaticDraw, static_cast<unsigned>(Countof(indices)), IndexFormat::UInt32, indices);
	m_vao = renderSystem.CreateVertexArray(m_vb, m_ib, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _004Quad::Destroy()
{
	m_shader.reset();
	m_vb.reset();
	m_ib.reset();
	m_vao.reset();
}
//-----------------------------------------------------------------------------
void _004Quad::Render()
{
	auto& renderSystem = GetRenderSystem();

	renderSystem.SetClearColor({ 0.0f, 0.64f, 0.91f });
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	renderSystem.ClearFrame();

	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.Draw(m_vao);
}
//-----------------------------------------------------------------------------
void _004Quad::Update(float /*deltaTime*/)
{
	if( GetInputSystem().IsKeyDown(Input::KEY_ESCAPE) )
	{
		ExitRequest();
		return;
	}

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 100.f);
}
//-----------------------------------------------------------------------------