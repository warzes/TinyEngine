#include "stdafx.h"
#include "005_BasicTexture.h"
//-----------------------------------------------------------------------------
namespace
{
	const char* vertexShaderText = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 ProjectionMatrix;

out vec2 TexCoord;

void main()
{
	gl_Position = ProjectionMatrix * vec4(aPosition, 1.0);
	TexCoord    = aTexCoord;
}
)";

	const char* fragmentShaderText = R"(
in vec2 TexCoord;
out vec4 FragmentColor;

uniform sampler2D DiffuseTexture;

void main()
{
	FragmentColor = texture(DiffuseTexture, TexCoord);
}
)";

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
	} vert[] =
	{
		{{ -0.5f,  0.5f, 2.0f}, {0.0f, 0.0f}}, // top left
		{{  0.5f,  0.5f, 2.0f}, {1.0f, 0.0f}}, // top right
		{{  0.5f, -0.5f, 2.0f}, {1.0f, 1.0f}}, // bottom right
		{{ -0.5f, -0.5f, 2.0f}, {0.0f, 1.0f}}, // bottom left
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 2,   // first triangle
		2, 1, 0    // second triangle
	};
}
//-----------------------------------------------------------------------------
bool _005BasicTexture::Create()
{
	glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "ProjectionMatrix");

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw,
		static_cast<unsigned>(Countof(vert)), static_cast<unsigned>(sizeof(Vertex)), vert,
		static_cast<unsigned>(Countof(indices)), IndexFormat::UInt32, indices,
		m_shader);

	m_texture = renderSystem.CreateTexture2D("../ExampleData/textures/1mx1m.png");

	return true;
}
//-----------------------------------------------------------------------------
void _005BasicTexture::Destroy()
{
	m_shader.reset();
	m_geom.reset();
	m_texture.reset();
}
//-----------------------------------------------------------------------------
void _005BasicTexture::Render()
{
	auto& renderSystem = GetRenderSystem();

	renderSystem.SetClearColor({ 0.0f, 0.64f, 0.91f });
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	renderSystem.ClearFrame();

	renderSystem.Bind(m_texture, 0);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform("DiffuseTexture", 0);
	renderSystem.Draw(m_geom);
}
//-----------------------------------------------------------------------------
void _005BasicTexture::Update(float /*deltaTime*/)
{
	if (GetInputSystem().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 100.f);
}
//-----------------------------------------------------------------------------