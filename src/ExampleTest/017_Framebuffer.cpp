#include "stdafx.h"
#include "017_Framebuffer.h"
//-----------------------------------------------------------------------------
bool _017Framebuffer::Create()
{
	auto& renderSystem = GetRenderSystem();

	{
		const char* vertexShaderText = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
TexCoords = aTexCoords;
gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

		const char* fragmentShaderText = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
FragColor = texture(texture1, TexCoords);
}
)";
		m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
		m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projection");
		m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "view");
		m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "model");
	}

	{
		const char* vertexShaderText = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
TexCoords = aTexCoords;
gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  
)";

		const char* fragmentShaderText = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	FragColor = vec4(average, average, average, 1.0);
} 
)";
		screenShader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	}

	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	m_geomBox = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(cubeVertices), (unsigned)sizeof(float), cubeVertices, m_shader);

	float planeVertices[] = {
		// positions          // texture Coords 
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	m_geomPlane = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(planeVertices), (unsigned)sizeof(float), planeVertices, m_shader);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
		// positions   // texCoords
		-0.3f,  1.0f,  0.0f, 1.0f,
		-0.3f,  0.5f,  0.0f, 0.0f,
		 0.3f,  0.5f,  1.0f, 0.0f,

		-0.3f,  1.0f,  0.0f, 1.0f,
		 0.3f,  0.5f,  1.0f, 0.0f,
		 0.3f,  1.0f,  1.0f, 1.0f
	};
	m_geomQuad = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(quadVertices), (unsigned)sizeof(float), quadVertices, screenShader);

	cubeTexture = renderSystem.CreateTexture2D("../ExampleData/textures/container2.png");
	floorTexture = renderSystem.CreateTexture2D("../ExampleData/textures/1mx1m.png");

	Texture2DCreateInfo createInfo;
	createInfo.format = TexelsFormat::RGB_U8;
	createInfo.width = (uint16_t)GetWindowWidth();
	createInfo.height = (uint16_t)GetWindowHeight();
	Texture2DInfo info;
	info.magFilter = TextureMagFilter::Linear;
	info.minFilter = TextureMinFilter::Linear;
	info.mipmap = false;
	m_textureColorbuffer = renderSystem.CreateTexture2D(createInfo, info);
	m_rbo = renderSystem.CreateRenderbuffer({ GetWindowWidth(),  GetWindowHeight() }, ImageFormat::D24S8);

	renderSystem.Bind(m_fbo);
	m_fbo = renderSystem.CreateFramebuffer(m_textureColorbuffer, m_rbo);

	GetInput().SetMouseLock(true);

	return true;
}
//-----------------------------------------------------------------------------
void _017Framebuffer::Destroy()
{
	GetInput().SetMouseLock(false);
	m_shader.reset();
	screenShader.reset();
	m_geomBox.reset();
	m_geomPlane.reset();
	m_geomQuad.reset();
	cubeTexture.reset();
	floorTexture.reset();

	m_textureColorbuffer.reset();
	m_rbo.reset();
	m_fbo.reset();
}
//-----------------------------------------------------------------------------
void _017Framebuffer::Render()
{
	auto& renderSystem = GetRenderSystem();
	
	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	glm::mat4 model = glm::mat4(1.0f);

	renderSystem.Bind(m_fbo);
	renderSystem.SetClearColor({ 0.1f, 0.1f, 0.1f });
	renderSystem.ClearFrame();
	glEnable(GL_DEPTH_TEST);

	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());

	// cubes
	renderSystem.Bind(cubeTexture, 0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
	renderSystem.SetUniform(m_uniformWorldMatrix, model);
	renderSystem.Draw(m_geomBox->vao);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
	renderSystem.SetUniform(m_uniformWorldMatrix, model);
	renderSystem.Draw(m_geomBox->vao);
	// floor
	renderSystem.Bind(floorTexture, 0);
	renderSystem.SetUniform(m_uniformWorldMatrix, glm::mat4(1.0f));
	renderSystem.Draw(m_geomPlane->vao);

	renderSystem.ResetState(ResourceType::Framebuffer);
	renderSystem.SetClearColor({ 0.1f, 0.1f, 0.1f });
	renderSystem.ClearFrame();
	// cubes
	renderSystem.Bind(cubeTexture, 0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
	renderSystem.SetUniform(m_uniformWorldMatrix, model);
	renderSystem.Draw(m_geomBox->vao);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
	renderSystem.SetUniform(m_uniformWorldMatrix, model);
	renderSystem.Draw(m_geomBox->vao);
	// floor
	renderSystem.Bind(floorTexture, 0);
	renderSystem.SetUniform(m_uniformWorldMatrix, glm::mat4(1.0f));
	renderSystem.Draw(m_geomPlane->vao);

	glDisable(GL_DEPTH_TEST);
	renderSystem.Bind(screenShader);
	renderSystem.Bind(m_fbo->colorTexture, 0);
	renderSystem.Draw(m_geomQuad->vao);
}
//-----------------------------------------------------------------------------
void _017Framebuffer::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		ExitRequest();
		return;
	}

	const float mouseSensitivity = 10.0f * deltaTime;
	const float moveSpeed = 10.0f * deltaTime;
	const glm::vec3 oldCameraPos = m_camera.position;

	if( GetInput().IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);

	glm::vec2 delta = GetInput().GetMouseDeltaPosition();
	if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
	if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------