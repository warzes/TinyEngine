#include "stdafx.h"
#include "Bumpmapping.h"
#include "Common/DemoPlane.h"
#include "Common/DemoCube.h"
#include "Common/NoiseGenerator.h"
//-----------------------------------------------------------------------------
namespace
{
	constexpr const char* vsTex = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 FragPosition;
out vec2 TexCoord;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(aPosition, 1.0);
	FragPosition = vec3(uWorld*vec4(aPosition, 1.0));
	TexCoord = aTexCoord;
}
)";

	constexpr const char* fsTex = R"(
in vec2 TexCoord;
in vec3 FragPosition;
out vec4 FragmentColor;

uniform vec3 CameraPosition;
uniform vec3 LightPosition;

uniform sampler2D DiffuseTexture;

void main()
{
	vec3 lightDir = normalize(LightPosition-FragPosition);
	float lightLength = length(LightPosition-FragPosition);
	float diffuseLight = 0.0;
	float specLight = 0.0;
	vec3 fragNormalTransformed = vec3(0.0, 1.0, 0.0);

	//find normal from texture
	const float EPSILON_DERIVATIVE = 1.0/1024.0;
	float pointXBefore = texture(DiffuseTexture, TexCoord+vec2(-EPSILON_DERIVATIVE, 0.0)).r;
	float pointXAfter = texture(DiffuseTexture, TexCoord+vec2(EPSILON_DERIVATIVE, 0.0)).r;
	float pointYBefore = texture(DiffuseTexture, TexCoord+vec2(0.0, -EPSILON_DERIVATIVE)).r;
	float pointYAfter = texture(DiffuseTexture, TexCoord+vec2(0.0, EPSILON_DERIVATIVE)).r;

	vec3 derX = normalize(vec3(2*EPSILON_DERIVATIVE, pointXAfter-pointXBefore, 0.0));
	vec3 derY = normalize(vec3(0.0, pointYAfter-pointYBefore, 2*EPSILON_DERIVATIVE));

	vec3 normal = normalize(cross(derY, derX));

	//by adding to the y component, we make the surface less "rough"
	fragNormalTransformed = normalize(normal+vec3(0.0, 1.5, 0.0));

	//calculations for specular light
	vec3 reflexion = 2*fragNormalTransformed*dot(fragNormalTransformed, lightDir)-lightDir;
	reflexion = normalize(reflexion);
	vec3 viewDir = normalize(CameraPosition-FragPosition);

	specLight = pow(max(dot(reflexion, viewDir), 0.0), 16);
	specLight = clamp(specLight, 0.0, 1.0);

	diffuseLight = dot(fragNormalTransformed, lightDir)/(lightLength/2.0);
	diffuseLight = clamp(diffuseLight, 0.0, 1.0);

	float lum = 1.0*diffuseLight+0.5*specLight;
	lum = clamp(lum, 0.0, 1.0);

	FragmentColor = vec4(vec3(lum), 1.0);
}
)";

	ShaderProgramRef shaderCube;
	GeometryBufferRef cube;
	glm::mat4 TransformCube;


	ShaderProgramRef shaderPlane;
	GeometryBufferRef plane;
	glm::mat4 TransformPlane;
	Texture2DRef texture;

	Camera camera;

	glm::vec3 lightPosition = { 0.0f, 2.0f, 0.0f };

	Noise_generator noise_gen;
	uint32_t noise_selection;
}
//-----------------------------------------------------------------------------
bool Bumpmapping::Create()
{
	auto& renderSystem = GetRenderSystem();

	shaderPlane = renderSystem.CreateShaderProgram({ vsTex }, { fsTex });
	plane = CreateDemoPlaneTexture(shaderPlane);
	//shaderPlane = CreateDemoPlaneShader();
	//plane = CreateDemoPlane(shaderPlane);

	TransformPlane = glm::mat4(1.0f);
	TransformPlane = glm::scale(TransformPlane, { 2.0f, 1.0f, 2.0f });
	TransformPlane = glm::translate(TransformPlane, { 0.0f, -1.0f, 0.0f });

	camera.Teleport({ 3.0f, 1.0f, -4.0f });
	camera.target = glm::vec3(0.0f);
	GetInputSystem().SetMouseLock(true);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	std::vector<std::vector<float> > plane_texture_data = noise_gen.get_2D_noise(512, 512, -1.0f, 1.0f, -1.0f, 1.0f);

	Texture2DCreateInfo createInfo;
	createInfo.width = plane_texture_data.size();
	createInfo.height = plane_texture_data[0].size();

	float* tempdata = new float[createInfo.width * createInfo.height];
	for (unsigned int i = 0; i < createInfo.width; i++) {
		for (unsigned int j = 0; j < createInfo.height; j++) {

			tempdata[i * createInfo.width + j] = plane_texture_data[i][j];
		}
	}
	createInfo.pixelData = new uint8_t[createInfo.width * createInfo.height * sizeof(float)];
	memcpy(createInfo.pixelData, tempdata, createInfo.width * createInfo.height * sizeof(float));

	createInfo.format = TexelsFormat::R_F32;

	Texture2DInfo info;
	info.mipmap = false;

	texture = renderSystem.CreateTexture2D(createInfo, info);

	delete[] tempdata;


	shaderCube = CreateDemoCubeShader();
	cube = CreateDemoCube(shaderCube);
	TransformCube = glm::mat4(1.0f);

	return true;
}
//-----------------------------------------------------------------------------
void Bumpmapping::Destroy()
{
	GetInputSystem().SetMouseLock(false);
	shaderPlane.reset();
	plane.reset();
}
//-----------------------------------------------------------------------------
void Bumpmapping::Render()
{
	auto& renderSystem = GetRenderSystem();

	renderSystem.SetClearColor({ 0.0f, 0.64f, 0.91f });
	renderSystem.SetViewport(GetWindowWidth(), GetWindowHeight());
	renderSystem.ClearFrame();

	renderSystem.Bind(shaderPlane);
	renderSystem.Bind(texture, 0);
	renderSystem.SetUniform("DiffuseTexture", 0);
	renderSystem.SetUniform("uProjection", m_perspective);
	renderSystem.SetUniform("uView", camera.GetViewMatrix());
	renderSystem.SetUniform("uWorld", TransformPlane);
	renderSystem.SetUniform("LightPosition", lightPosition);
	renderSystem.SetUniform("CameraPosition", camera.position);
	renderSystem.Draw(plane);
}
//-----------------------------------------------------------------------------
void Bumpmapping::Update(float deltaTime)
{
	auto& input = GetInputSystem();
	if (input.IsKeyDown(Input::KEY_ESCAPE))
	{
		ExitRequest();
		return;
	}

	m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 100.f);

	lightPosition[0] = 2.0 * cos(glfwGetTime());
	lightPosition[2] = 2.0 * sin(glfwGetTime());

	const float mouseSensitivity = 10.0f * deltaTime;
	const float moveSpeed = 10.0f * deltaTime;
	const glm::vec3 oldCameraPos = camera.position;

	if (input.IsKeyDown(Input::KEY_W)) camera.MoveBy(moveSpeed);
	if (input.IsKeyDown(Input::KEY_S)) camera.MoveBy(-moveSpeed);
	if (input.IsKeyDown(Input::KEY_A)) camera.StrafeBy(moveSpeed);
	if (input.IsKeyDown(Input::KEY_D)) camera.StrafeBy(-moveSpeed);

	glm::vec2 delta = input.GetMouseDeltaPosition();
	if (delta.x != 0.0f) camera.RotateLeftRight(delta.x * mouseSensitivity);
	if (delta.y != 0.0f) camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------