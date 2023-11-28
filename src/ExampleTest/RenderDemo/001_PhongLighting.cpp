#include "stdafx.h"
#include "001_PhongLighting.h"
//-----------------------------------------------------------------------------
namespace
{
	constexpr const char* vertexShaderText = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 FragmentPosition;
out vec3 Normal;

void main()
{
	Normal = mat3(transpose(inverse(uWorld))) * aNormal;
	FragmentPosition = vec3(uWorld * vec4(aPosition, 1.0));
	gl_Position = uProjection * uView * vec4(FragmentPosition, 1.0);
}
)";

	constexpr const char* fragmentShaderText = R"(
out vec4 FragmentColor;

in vec3 FragmentPosition;
in vec3 Normal;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};  
uniform Material uMaterial;

struct Light {
	vec3 position;
  
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light uLight;

uniform vec3 uViewPos;

void main()
{
	// ambient
	vec3 ambient = uLight.ambient * uMaterial.ambient;

	// diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(uLight.position - FragmentPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

	// specular
	vec3 viewDir = normalize(uViewPos - FragmentPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
	vec3 specular = uLight.specular * (spec * uMaterial.specular);

	vec3 result = ambient + diffuse + specular;
	FragmentColor = vec4(result, 1.0);
}
)";

	constexpr const char* lightSourceVertexShaderText = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(aPosition, 1.0);
}
)";

	constexpr const char* lightSourceFragmentShaderText = R"(
out vec4 FragmentColor;

void main()
{
	FragmentColor = vec4(1.0);
}
)";

	glm::vec3 lightPos(1.2f, 1.0f, -2.0f);

	float vertices[] =
	{
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
}
//-----------------------------------------------------------------------------
bool _001PhongLighting::Create()
{
	auto& renderSystem = GetRenderSystem();

	m_shaderObject = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformObjectProjectionMatrix = renderSystem.GetUniform(m_shaderObject, "uProjection");
	m_uniformObjectViewMatrix = renderSystem.GetUniform(m_shaderObject, "uView");
	m_uniformObjectWorldMatrix = renderSystem.GetUniform(m_shaderObject, "uWorld");
	m_uniformObjectViewPos = renderSystem.GetUniform(m_shaderObject, "uViewPos");

	m_shaderLightSource = renderSystem.CreateShaderProgram({ lightSourceVertexShaderText }, { lightSourceFragmentShaderText });
	m_uniformLightSourceProjectionMatrix = renderSystem.GetUniform(m_shaderLightSource, "uProjection");
	m_uniformLightSourceViewMatrix = renderSystem.GetUniform(m_shaderLightSource, "uView");
	m_uniformLightSourceWorldMatrix = renderSystem.GetUniform(m_shaderLightSource, "uWorld");

	const std::vector<VertexAttribute> formatVertex =
	{
		{.location = 0, .size = 3, .normalized = false, .stride = sizeof(float) * 6, .offset = (void*)0},
		{.location = 1, .size = 3, .normalized = false, .stride = sizeof(float) * 6, .offset = (void*)(3 * sizeof(float))},
	};

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, static_cast<unsigned>(Countof(vertices))/6, static_cast<unsigned>(sizeof(float))*6, vertices, formatVertex);

	m_camera.Teleport({ 0.0f, 0.7f, -3.0f });

	GetInputSystem().SetMouseLock(true);
	return true;
}
//-----------------------------------------------------------------------------
void _001PhongLighting::Destroy()
{
	GetInputSystem().SetMouseLock(false);
	m_shaderObject.reset();
	m_shaderLightSource.reset();
	m_geom.reset();
}
//-----------------------------------------------------------------------------
void _001PhongLighting::Render()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	renderSystem.SetClearColor({ 0.0f, 0.64f, 0.91f });
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	renderSystem.ClearFrame();

	// Draw object

	renderSystem.Bind(m_shaderObject);
	renderSystem.SetUniform(m_uniformObjectProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformObjectViewMatrix, m_camera.GetViewMatrix());
	renderSystem.SetUniform(m_uniformObjectWorldMatrix, glm::mat4(1.0f));

	renderSystem.SetUniform(m_uniformObjectViewPos, m_camera.position);

	renderSystem.SetUniform("uLight.position", lightPos);
	renderSystem.SetUniform("uLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	renderSystem.SetUniform("uLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	renderSystem.SetUniform("uLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	
	renderSystem.SetUniform("uMaterial.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
	renderSystem.SetUniform("uMaterial.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
	renderSystem.SetUniform("uMaterial.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	renderSystem.SetUniform("uMaterial.shininess", 32.0f);


	renderSystem.Draw(m_geom);

	// Draw light source

	auto model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

	renderSystem.Bind(m_shaderLightSource);
	renderSystem.SetUniform(m_uniformLightSourceProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformLightSourceViewMatrix, m_camera.GetViewMatrix());
	renderSystem.SetUniform(m_uniformLightSourceWorldMatrix, model);
	renderSystem.Draw(m_geom);
}
//-----------------------------------------------------------------------------
void _001PhongLighting::Update(float deltaTime)
{
	if (GetInputSystem().IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);

	const float mouseSensitivity = 10.0f * deltaTime;
	const float moveSpeed = 10.0f * deltaTime;
	const glm::vec3 oldCameraPos = m_camera.position;

	if (GetInputSystem().IsKeyDown(Input::KEY_W)) m_camera.MoveBy(moveSpeed);
	if (GetInputSystem().IsKeyDown(Input::KEY_S)) m_camera.MoveBy(-moveSpeed);
	if (GetInputSystem().IsKeyDown(Input::KEY_A)) m_camera.StrafeBy(moveSpeed);
	if (GetInputSystem().IsKeyDown(Input::KEY_D)) m_camera.StrafeBy(-moveSpeed);

	glm::vec2 delta = GetInputSystem().GetMouseDeltaPosition();
	if (delta.x != 0.0f)  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
	if (delta.y != 0.0f)  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------