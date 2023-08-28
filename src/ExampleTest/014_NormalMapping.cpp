#include "stdafx.h"
#include "014_NormalMapping.h"
//-----------------------------------------------------------------------------
struct _014LightClass
{
	glm::vec4 diffuseColor;
	glm::vec3 direction;
};
_014LightClass m_diffuse;
//-----------------------------------------------------------------------------
struct _014TempVertex
{
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
};
//-----------------------------------------------------------------------------
struct _014Vertex
{
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 binormal;
};
_014Vertex vert[36];
struct TempVertexType
{
	glm::vec3 pos;
	glm::vec2 texCoord;
};
//-----------------------------------------------------------------------------
void CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, glm::vec3& tangent, glm::vec3& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.pos.x - vertex1.pos.x;
	vector1[1] = vertex2.pos.y - vertex1.pos.y;
	vector1[2] = vertex2.pos.z - vertex1.pos.z;

	vector2[0] = vertex3.pos.x - vertex1.pos.x;
	vector2[1] = vertex3.pos.y - vertex1.pos.y;
	vector2[2] = vertex3.pos.z - vertex1.pos.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.texCoord.x - vertex1.texCoord.x;
	tvVector[0] = vertex2.texCoord.y - vertex1.texCoord.y;

	tuVector[1] = vertex3.texCoord.x - vertex1.texCoord.x;
	tvVector[1] = vertex3.texCoord.y - vertex1.texCoord.y;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}
//-----------------------------------------------------------------------------
void CalculateModelVectors(unsigned vertexCount)
{
	unsigned faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	glm::vec3 tangent, binormal;


	// Calculate the number of faces in the model.
	faceCount = vertexCount / 3u;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent and binormal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.pos.x = vert[index].pos.x;
		vertex1.pos.y = vert[index].pos.y;
		vertex1.pos.z = vert[index].pos.z;
		vertex1.texCoord.x = vert[index].texCoord.x;
		vertex1.texCoord.y = vert[index].texCoord.y;
		index++;

		vertex2.pos.x = vert[index].pos.x;
		vertex2.pos.y = vert[index].pos.y;
		vertex2.pos.z = vert[index].pos.z;
		vertex2.texCoord.x = vert[index].texCoord.x;
		vertex2.texCoord.y = vert[index].texCoord.y;
		index++;

		vertex3.pos.x = vert[index].pos.x;
		vertex3.pos.y = vert[index].pos.y;
		vertex3.pos.z = vert[index].pos.z;
		vertex3.texCoord.x = vert[index].texCoord.x;
		vertex3.texCoord.y = vert[index].texCoord.y;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		vert[index - 1].tangent.x = tangent.x;
		vert[index - 1].tangent.y = tangent.y;
		vert[index - 1].tangent.z = tangent.z;
		vert[index - 1].binormal.x = binormal.x;
		vert[index - 1].binormal.y = binormal.y;
		vert[index - 1].binormal.z = binormal.z;

		vert[index - 2].tangent.x = tangent.x;
		vert[index - 2].tangent.y = tangent.y;
		vert[index - 2].tangent.z = tangent.z;
		vert[index - 2].binormal.x = binormal.x;
		vert[index - 2].binormal.y = binormal.y;
		vert[index - 2].binormal.z = binormal.z;

		vert[index - 3].tangent.x = tangent.x;
		vert[index - 3].tangent.y = tangent.y;
		vert[index - 3].tangent.z = tangent.z;
		vert[index - 3].binormal.x = binormal.x;
		vert[index - 3].binormal.y = binormal.y;
		vert[index - 3].binormal.z = binormal.z;
	}
}
//-----------------------------------------------------------------------------
bool _014NormalMapping::Create()
{
	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexCoord;
layout(location = 2) in vec3 inputNormal;
layout(location = 3) in vec3 inputTangent;
layout(location = 4) in vec3 inputBinormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 texCoord;
out vec3 normal;
out vec3 tangent;
out vec3 binormal;

void main()
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(inputPosition, 1.0f);

	// Store the texture coordinates for the pixel shader.
	texCoord = inputTexCoord;

	// Calculate the normal vector against the world matrix only and then normalize the final value.
	normal = mat3(worldMatrix) * inputNormal;
	normal = normalize(normal);

	// Calculate the tangent vector against the world matrix only and then normalize the final value.
	tangent = mat3(worldMatrix) * inputTangent;
	tangent = normalize(tangent);

	// Calculate the binormal vector against the world matrix only and then normalize the final value.
	binormal = mat3(worldMatrix) * inputBinormal;
	binormal = normalize(binormal);
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 tangent;
in vec3 binormal;

out vec4 outputColor;

uniform sampler2D shaderTexture1;
uniform sampler2D shaderTexture2;
uniform vec3 lightDirection;
uniform vec4 diffuseLightColor;

void main()
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	vec4 textureColor = texture(shaderTexture1, texCoord);

	// Sample the pixel from the normal map.
	vec4 bumpMap = texture(shaderTexture2, texCoord);

	// Expand the range of the normal value from (0, +1) to (-1, +1).
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the normal map.
	vec3 bumpNormal = (bumpMap.x * tangent) + (bumpMap.y * binormal) + (bumpMap.z * normal);

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	// Invert the light direction for calculations.
	vec3 lightDir = -lightDirection;

	// Calculate the amount of light on this pixel based on the normal map value.
	float lightIntensity = clamp(dot(bumpNormal, lightDir), 0.0f, 1.0f);

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	outputColor =  clamp((diffuseLightColor * lightIntensity), 0.0f, 1.0f);

	// Combine the final light color with the texture color.
	outputColor = outputColor * textureColor;
}
)";

	_014TempVertex tmpvert[] =
	{
{{-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}},
{{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}},
{{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
{{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
{{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}},
{{ 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
{{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f}, { 1.0f,  0.0f,  0.0f}},
{{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, { 1.0f,  0.0f,  0.0f}},
{{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, { 1.0f,  0.0f,  0.0f}},
{{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, { 1.0f,  0.0f,  0.0f}},
{{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, { 1.0f,  0.0f,  0.0f}},
{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}, { 1.0f,  0.0f,  0.0f}},
{{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f}, { 0.0f,  0.0f,  1.0f}},
{{-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, { 0.0f,  0.0f,  1.0f}},
{{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}, { 0.0f,  0.0f,  1.0f}},
{{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}, { 0.0f,  0.0f,  1.0f}},
{{-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, { 0.0f,  0.0f,  1.0f}},
{{-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}, { 0.0f,  0.0f,  1.0f}},
{{-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},
{{-1.0f,  1.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},
{{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},
{{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},
{{-1.0f,  1.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},
{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},
{{-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f}, { 0.0f,  1.0f,  0.0f}},
{{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, { 0.0f,  1.0f,  0.0f}},
{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
{{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, { 0.0f,  1.0f,  0.0f}},
{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}, { 0.0f, -1.0f,  0.0f}},
{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}, { 0.0f, -1.0f,  0.0f}},
{{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
{{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}, { 0.0f, -1.0f,  0.0f}},
{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
	};

	for (size_t i = 0; i < 36; i++)
	{
		vert[i].pos = tmpvert[i].pos;
		vert[i].texCoord = tmpvert[i].texCoord;
		vert[i].normal = tmpvert[i].normal;
	}
	CalculateModelVectors(36);

	//glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");
	m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "viewMatrix");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "worldMatrix");
	m_uniformLightDirection = renderSystem.GetUniform(m_shader, "lightDirection");
	m_uniformLightColor = renderSystem.GetUniform(m_shader, "diffuseLightColor");

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(vert), (unsigned)sizeof(_014Vertex), vert, m_shader);

	m_texture1 = renderSystem.CreateTexture2D("../ExampleData/textures/stone01.png");
	m_texture2 = renderSystem.CreateTexture2D("../ExampleData/textures/normal01.png");

	m_diffuse.diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_diffuse.direction = glm::vec3(0.0f, 0.0f, 1.0f);

	m_camera.Teleport(glm::vec3(0.0f, 0.0f, -5.0f));

	return true;
}
//-----------------------------------------------------------------------------
void _014NormalMapping::Destroy()
{
	m_shader.reset();
	m_geom.reset();
	m_texture1.reset();
	m_texture2.reset();
}
//-----------------------------------------------------------------------------
void _014NormalMapping::Render()
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

	renderSystem.SetClearColor(glm::vec3(0.0f));
	renderSystem.ClearFrame();
	renderSystem.Bind(m_texture1, 0);
	renderSystem.Bind(m_texture2, 1);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());
	renderSystem.SetUniform(m_uniformWorldMatrix, rotationY);
	renderSystem.SetUniform(m_uniformLightDirection, m_diffuse.direction);
	renderSystem.SetUniform(m_uniformLightColor, m_diffuse.diffuseColor);
	renderSystem.SetUniform("shaderTexture1", 0);
	renderSystem.SetUniform("shaderTexture2", 1);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _014NormalMapping::Update(float deltaTime)
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