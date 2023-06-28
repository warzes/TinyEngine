#include "stdafx.h"
#include "005_BasicTexture.h"
//-----------------------------------------------------------------------------
bool _005BasicTexture::Create()
{
	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projectionMatrix;

out vec2 TexCoord;

void main()
{
	gl_Position = projectionMatrix * vec4(aPosition, 1.0);
	TexCoord    = aTexCoord;
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
	FragColor = texture(Texture, TexCoord);
}
)";

	struct testVertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
	}
	vert[] =
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

	glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(vert), (unsigned)sizeof(testVertex), vert, (unsigned)Countof(indices), IndexFormat::UInt32, indices, m_shader);

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

	if (m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight())
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	renderSystem.ClearFrame();
	renderSystem.Bind(m_texture, 0);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform("Texture", 0);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _005BasicTexture::Update(float /*deltaTime*/)
{
	if (GetInput().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------