#include "stdafx.h"
#include "012_MultiplePointLights.h"
// TODO: доделать
//-----------------------------------------------------------------------------
bool _012MultiplePointLights::Create()
{
	const char* vertexShaderText = R"(
#version 330 core

#define NUM_LIGHTS 4

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexCoord;
layout(location = 2) in vec3 inputNormal;


uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition[NUM_LIGHTS];

out vec2 texCoord;
out vec3 normal;
out vec3 lightPos[NUM_LIGHTS];

void main()
{
	vec4 worldPosition;
	int i;


	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;

	// Store the texture coordinates for the pixel shader.
	texCoord = inputTexCoord;

	// Calculate the normal vector against the world matrix only.
	normal = mat3(worldMatrix) * inputNormal;

	// Normalize the normal vector.
	normal = normalize(normal);

	// Calculate the position of the vertex in the world.
	worldPosition = worldMatrix * vec4(inputPosition, 1.0f);

	for(i=0; i<NUM_LIGHTS; i++)
	{
		// Determine the light positions based on the position of the lights and the position of the vertex in the world.
		lightPos[i] = lightPosition[i].xyz - worldPosition.xyz;

		// Normalize the light position vectors.
		lightPos[i] = normalize(lightPos[i]);
	}
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

#define NUM_LIGHTS 4

in vec2 texCoord;
in vec3 normal;
in vec3 lightPos[NUM_LIGHTS];

out vec4 outputColor;

uniform sampler2D shaderTexture;
uniform vec4 diffuseColor[NUM_LIGHTS];

void main()
{
	float lightIntensity[NUM_LIGHTS];
	vec4 color[NUM_LIGHTS];
	vec4 colorSum;
	vec4 textureColor;
	int i;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = texture(shaderTexture, texCoord);

	for(i=0; i<NUM_LIGHTS; i++)
	{
		// Calculate the different amounts of light on this pixel based on the positions of the lights.
		lightIntensity[i] = clamp(dot(normal, lightPos[i]), 0.0f, 1.0f);

		// Determine the diffuse color amount of each of the four lights.
		color[i] = diffuseColor[i] * lightIntensity[i];
	}

	// Initialize the sum of colors.
	colorSum = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Add all of the light colors up.
	for(i=0; i<NUM_LIGHTS; i++)
	{
		colorSum.r += color[i].r;
		colorSum.g += color[i].g;
		colorSum.b += color[i].b;
	}

	// Multiply the texture pixel by the combination of all four light colors to get the final result.
	outputColor = clamp(colorSum, 0.0f, 1.0f) * textureColor;
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
{ {-5.0f,  0.0f, -5.0f}, {1.0f, 0.0f},  { 0.0f, -1.0f, 0.0f} },
{ { 5.0f,  0.0f, -5.0f}, {1.0f, 1.0f},  { 0.0f, -1.0f, 0.0f} },
{ { 5.0f,  0.0f,  5.0f}, {0.0f, 1.0f},  { 0.0f, -1.0f, 0.0f} },
{ { 5.0f,  0.0f,  5.0f}, {0.0f, 1.0f},  { 0.0f, -1.0f, 0.0f} },
{ {-5.0f,  0.0f,  5.0f}, {0.0f, 0.0f},  { 0.0f, -1.0f, 0.0f} },
{ {-5.0f,  0.0f, -5.0f}, {1.0f, 0.0f},  { 0.0f, -1.0f, 0.0f} },
	};

	//glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");
	m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "viewMatrix");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "worldMatrix");
	m_uniformLightPosition = renderSystem.GetUniform(m_shader, "lightPosition");
	m_uniformDiffuseColor = renderSystem.GetUniform(m_shader, "diffuseColor");

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(vert), (unsigned)sizeof(vertex), vert, m_shader);

	m_texture = renderSystem.CreateTexture2D("../ExampleData/textures/stone01.png");

	m_numLights = 4;
	m_light = new _012LightClass[m_numLights];
	// Manually set the color and position of each light.
	m_light[0].diffuseColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red
	m_light[0].position = glm::vec3(-3.0f, 0.0f, 3.0f);

	m_light[1].diffuseColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
	m_light[1].position = glm::vec3(3.0f, 0.0f, 3.0f);

	m_light[2].diffuseColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);  // Blue
	m_light[2].position = glm::vec3(-3.0f, 0.0f, -3.0f);

	m_light[3].diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
	m_light[3].position = glm::vec3(3.0f, 0.0f, -3.0f);

	m_camera.Teleport(glm::vec3(0.0f, 4.0f, -12.0f));

	return true;
}
//-----------------------------------------------------------------------------
void _012MultiplePointLights::Destroy()
{
	delete[] m_light;
	m_shader.reset();
	m_geom.reset();
	m_texture.reset();
}
//-----------------------------------------------------------------------------
void _012MultiplePointLights::Render()
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

	float diffuseColor[4], diffuseColorArray[16];
	float lightPosition[3], lightPositionArray[12];
	int j = 0;
	int k = 0;
	for (unsigned i = 0; i < m_numLights; i++)
	{
		// Get the color and position of each light.
		m_light[i].GetDiffuseColor(diffuseColor);
		m_light[i].GetPosition(lightPosition);

		// Copy the individual light data into the single larger arrays.
		diffuseColorArray[j + 0] = diffuseColor[0];
		diffuseColorArray[j + 1] = diffuseColor[1];
		diffuseColorArray[j + 2] = diffuseColor[2];
		diffuseColorArray[j + 3] = diffuseColor[3];
		j += 4;

		lightPositionArray[k + 0] = lightPosition[0];
		lightPositionArray[k + 1] = lightPosition[1];
		lightPositionArray[k + 2] = lightPosition[2];
		k += 3;
	}

	renderSystem.ClearFrame();
	renderSystem.Bind(m_texture, 0);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());
	renderSystem.SetUniform(m_uniformWorldMatrix, rotationY);
	renderSystem.SetUniform3(m_uniformLightPosition, m_numLights, lightPositionArray);
	renderSystem.SetUniform4(m_uniformDiffuseColor, m_numLights, diffuseColorArray);
	renderSystem.SetUniform("shaderTexture", 0);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _012MultiplePointLights::Update(float deltaTime)
{
	if (GetInputSystem().IsKeyDown(Input::KEY_ESCAPE))
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