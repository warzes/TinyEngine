#include "stdafx.h"
#include "008_AlphaMapping.h"
//-----------------------------------------------------------------------------
bool _008AlphaMapping::Create()
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
out vec4 outputColor;

uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;

void main()
{
	// Get the pixel color from the first texture.
	vec4 color1 = texture(Texture1, TexCoord);

	// Get the pixel color from the second texture.
	vec4 color2 = texture(Texture2, TexCoord);

	// Get the pixel color from the alpha texture.
	vec4 alphaValue = texture(Texture3, TexCoord);

	// Combine the two textures based on the alpha value.
	vec4 blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);

	// Saturate the final color value.
	outputColor = clamp(blendColor, 0.0f, 1.0f);
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

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(vert), (unsigned)sizeof(testVertex), vert, (unsigned)Countof(indices), IndexType::Uint32, indices, m_shader);

	m_texture1 = renderSystem.CreateTexture2D("../ExampleData/textures/stone01.png");
	m_texture2 = renderSystem.CreateTexture2D("../ExampleData/textures/dirt01.png");
	m_texture3 = renderSystem.CreateTexture2D("../ExampleData/textures/alpha01.png");

	return true;
}
//-----------------------------------------------------------------------------
void _008AlphaMapping::Destroy()
{
	m_shader.reset();
	m_geom.reset();
	m_texture1.reset();
	m_texture2.reset();
	m_texture3.reset();
}
//-----------------------------------------------------------------------------
void _008AlphaMapping::Render()
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
	renderSystem.Bind(m_texture1, 0);
	renderSystem.Bind(m_texture2, 1);
	renderSystem.Bind(m_texture3, 2);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform("Texture1", 0);
	renderSystem.SetUniform("Texture2", 1);
	renderSystem.SetUniform("Texture3", 2);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _008AlphaMapping::Update(float /*deltaTime*/)
{
	if (GetInput().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------