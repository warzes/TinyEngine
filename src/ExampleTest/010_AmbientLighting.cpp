#include "stdafx.h"
#include "010_AmbientLighting.h"
//-----------------------------------------------------------------------------
bool _010AmbientLighting::Create()
{
	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexCoord;
layout(location = 2) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 texCoord;
out vec3 normal;

void main()
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(inputPosition, 1.0f);

	// Store the texture coordinates for the pixel shader.
	texCoord = inputTexCoord;

	// Calculate the normal vector against the world matrix only.
	normal = mat3(worldMatrix) * inputNormal;

	// Normalize the normal vector.
	normal = normalize(normal);
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

in vec2 texCoord;
in vec3 normal;

out vec4 outputColor;

uniform sampler2D shaderTexture;
uniform vec3 lightDirection;
uniform vec4 diffuseLightColor;
uniform vec4 ambientLight;

void main()
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	vec4 textureColor = texture(shaderTexture, texCoord);

	// Set the default output color to the ambient light value for all pixels.
	vec4 color = ambientLight;

	// Invert the light direction for calculations.
	vec3 lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	float lightIntensity = clamp(dot(normal, lightDir), 0.0f, 1.0f);

	if(lightIntensity > 0.0f)
	{
		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		color += (diffuseLightColor * lightIntensity);
	}

	// Clamp the final light color.
	color = clamp(color, 0.0f, 1.0f);

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	outputColor = color * textureColor;
}
)";

	struct vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 normal;
	}
	vert[] =
	{
{ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f},  { 0.0f,  0.0f, -1.0f} },
{ { 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f},  { 0.0f,  0.0f, -1.0f} },
{ {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f},  { 0.0f,  0.0f, -1.0f} },
{ {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f},  { 0.0f,  0.0f, -1.0f} },
{ { 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f},  { 0.0f,  0.0f, -1.0f} },
{ { 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f},  { 0.0f,  0.0f, -1.0f} },
{ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f},  { 1.0f,  0.0f,  0.0f} },
{ { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f},  { 1.0f,  0.0f,  0.0f} },
{ { 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f},  { 1.0f,  0.0f,  0.0f} },
{ { 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f},  { 1.0f,  0.0f,  0.0f} },
{ { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f},  { 1.0f,  0.0f,  0.0f} },
{ { 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f},  { 1.0f,  0.0f,  0.0f} },
{ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f},  { 0.0f,  0.0f,  1.0f} },
{ {-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f},  { 0.0f,  0.0f,  1.0f} },
{ { 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f},  { 0.0f,  0.0f,  1.0f} },
{ { 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f},  { 0.0f,  0.0f,  1.0f} },
{ {-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f},  { 0.0f,  0.0f,  1.0f} },
{ {-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f},  { 0.0f,  0.0f,  1.0f} },
{ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f},  {-1.0f,  0.0f,  0.0f} },
{ {-1.0f,  1.0f, -1.0f}, {1.0f, 0.0f},  {-1.0f,  0.0f,  0.0f} },
{ {-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f},  {-1.0f,  0.0f,  0.0f} },
{ {-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f},  {-1.0f,  0.0f,  0.0f} },
{ {-1.0f,  1.0f, -1.0f}, {1.0f, 0.0f},  {-1.0f,  0.0f,  0.0f} },
{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f},  {-1.0f,  0.0f,  0.0f} },
{ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f},  { 0.0f,  1.0f,  0.0f} },
{ { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f},  { 0.0f,  1.0f,  0.0f} },
{ {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f},  { 0.0f,  1.0f,  0.0f} },
{ {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f},  { 0.0f,  1.0f,  0.0f} },
{ { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f},  { 0.0f,  1.0f,  0.0f} },
{ { 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f},  { 0.0f,  1.0f,  0.0f} },
{ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f},  { 0.0f, -1.0f,  0.0f} },
{ { 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f},  { 0.0f, -1.0f,  0.0f} },
{ {-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f},  { 0.0f, -1.0f,  0.0f} },
{ {-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f},  { 0.0f, -1.0f,  0.0f} },
{ { 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f},  { 0.0f, -1.0f,  0.0f} },
{ { 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f},  { 0.0f, -1.0f,  0.0f} },
	};

	//glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");
	m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "viewMatrix");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "worldMatrix");
	m_uniformLightDirection = renderSystem.GetUniform(m_shader, "lightDirection");
	m_uniformLightDiffuseColor = renderSystem.GetUniform(m_shader, "diffuseLightColor");
	m_uniformLightAmbientColor = renderSystem.GetUniform(m_shader, "ambientLight");

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(vert), (unsigned)sizeof(vertex), vert, m_shader);

	m_texture = renderSystem.CreateTexture2D("../ExampleData/textures/stone01.png");

	m_light.diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_light.direction = glm::vec3(1.0f, 0.0f, 0.0f);
	m_light.ambientLight = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);

	m_camera.Teleport(glm::vec3(0.0f, 0.0f, -5.0f));

	return true;
}
//-----------------------------------------------------------------------------
void _010AmbientLighting::Destroy()
{
	m_shader.reset();
	m_geom.reset();
	m_texture.reset();
}
//-----------------------------------------------------------------------------
void _010AmbientLighting::Render()
{
	auto& renderSystem = GetRenderSystem();

	if (m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight())
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0.0f, 1.0f, 0.0f));

	renderSystem.ClearFrame();
	renderSystem.Bind(m_texture, 0);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());
	renderSystem.SetUniform(m_uniformWorldMatrix, rotationY);
	renderSystem.SetUniform(m_uniformLightDirection, m_light.direction);
	renderSystem.SetUniform(m_uniformLightDiffuseColor, m_light.diffuseColor);
	renderSystem.SetUniform(m_uniformLightAmbientColor, m_light.ambientLight);
	renderSystem.SetUniform("shaderTexture", 0);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _010AmbientLighting::Update(float deltaTime)
{
	if (GetInput().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}

	m_rotation -= 0.0174532925f * 3000.0f * deltaTime;
	if (m_rotation <= 0.0f)
	{
		m_rotation += 360.0f;
	}
}
//-----------------------------------------------------------------------------