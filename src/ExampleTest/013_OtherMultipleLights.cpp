#include "stdafx.h"
#include "013_OtherMultipleLights.h"
//-----------------------------------------------------------------------------
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
// positions of the point lights
glm::vec3 pointLightPositions[] = {
glm::vec3(0.7f,  0.2f,  2.0f),
glm::vec3(2.3f, -3.3f, -4.0f),
glm::vec3(-4.0f,  2.0f, -12.0f),
glm::vec3(0.0f,  0.0f, -3.0f)
};
// positions all containers
glm::vec3 cubePositions[] = {
glm::vec3(0.0f,  0.0f,  0.0f),
glm::vec3(2.0f,  5.0f, -15.0f),
glm::vec3(-1.5f, -2.2f, -2.5f),
glm::vec3(-3.8f, -2.0f, -12.3f),
glm::vec3(2.4f, -0.4f, -3.5f),
glm::vec3(-1.7f,  3.0f, -7.5f),
glm::vec3(1.3f, -2.0f, -2.5f),
glm::vec3(1.5f,  2.0f, -2.5f),
glm::vec3(1.5f,  0.2f, -1.5f),
glm::vec3(-1.3f,  1.0f, -1.5f)
};
//-----------------------------------------------------------------------------
bool _013OtherMultipleLights::Create()
{
    auto& renderSystem = GetRenderSystem();

    const char* multipleLightsVertexShaderText = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
FragPos = vec3(model * vec4(aPos, 1.0));
Normal = mat3(transpose(inverse(model))) * aNormal;  
TexCoords = aTexCoords;
    
gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

    const char* multipleLightsFragmentShaderText = R"(
#version 330 core
out vec4 FragColor;

struct Material {
sampler2D diffuse;
sampler2D specular;
float shininess;
}; 

struct DirLight {
vec3 direction;
	
vec3 ambient;
vec3 diffuse;
vec3 specular;
};

struct PointLight {
vec3 position;
    
float constant;
float linear;
float quadratic;
	
vec3 ambient;
vec3 diffuse;
vec3 specular;
};

struct SpotLight {
vec3 position;
vec3 direction;
float cutOff;
float outerCutOff;
  
float constant;
float linear;
float quadratic;
  
vec3 ambient;
vec3 diffuse;
vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
// properties
vec3 norm = normalize(Normal);
vec3 viewDir = normalize(viewPos - FragPos);
    
// == =====================================================
// Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
// For each phase, a calculate function is defined that calculates the corresponding color
// per lamp. In the main() function we take all the calculated colors and sum them up for
// this fragment's final color.
// == =====================================================
// phase 1: directional lighting
vec3 result = CalcDirLight(dirLight, norm, viewDir);
// phase 2: point lights
for(int i = 0; i < NR_POINT_LIGHTS; i++)
result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
// phase 3: spot light
result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
vec3 lightDir = normalize(-light.direction);
// diffuse shading
float diff = max(dot(normal, lightDir), 0.0);
// specular shading
vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
// combine results
vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
vec3 lightDir = normalize(light.position - fragPos);
// diffuse shading
float diff = max(dot(normal, lightDir), 0.0);
// specular shading
vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
// attenuation
float distance = length(light.position - fragPos);
float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
// combine results
vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
ambient *= attenuation;
diffuse *= attenuation;
specular *= attenuation;
return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
vec3 lightDir = normalize(light.position - fragPos);
// diffuse shading
float diff = max(dot(normal, lightDir), 0.0);
// specular shading
vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
// attenuation
float distance = length(light.position - fragPos);
float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
// spotlight intensity
float theta = dot(lightDir, normalize(-light.direction)); 
float epsilon = light.cutOff - light.outerCutOff;
float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
// combine results
vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
ambient *= attenuation * intensity;
diffuse *= attenuation * intensity;
specular *= attenuation * intensity;
return (ambient + diffuse + specular);
}
)";

    m_lightingShader = renderSystem.CreateShaderProgram({ multipleLightsVertexShaderText }, { multipleLightsFragmentShaderText });

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    m_cube = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(vertices) / 8, sizeof(float) * 8, vertices, m_lightingShader);

    m_diffuseMap = renderSystem.CreateTexture2D("../ExampleData/textures/container2.png");
    m_specularMap = renderSystem.CreateTexture2D("../ExampleData/textures/container2_specular.png");

    //glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки
    GetInput().SetMouseLock(true);
    return true;
}
//-----------------------------------------------------------------------------
void _013OtherMultipleLights::Destroy()
{
    GetInput().SetMouseLock(false);
    m_lightingShader.reset();
    m_cube.reset();
    m_diffuseMap.reset();
    m_specularMap.reset();
}
//-----------------------------------------------------------------------------
void _013OtherMultipleLights::Render()
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
    renderSystem.Bind(m_lightingShader);
    renderSystem.SetUniform("material.diffuse", 0);
    renderSystem.SetUniform("material.specular", 1);
    renderSystem.SetUniform("viewPos", m_camera.position);
    renderSystem.SetUniform("material.shininess", 32.0f);

    /*
    Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
    the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
    by defining light types as classes and set their values in there, or by using a more efficient uniform approach
    by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
    */
    // directional light
    renderSystem.SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    renderSystem.SetUniform("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    renderSystem.SetUniform("dirLight.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
    renderSystem.SetUniform("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    // point light 1
    renderSystem.SetUniform("pointLights[0].position", pointLightPositions[0]);
    renderSystem.SetUniform("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    renderSystem.SetUniform("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    renderSystem.SetUniform("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    renderSystem.SetUniform("pointLights[0].constant", 1.0f);
    renderSystem.SetUniform("pointLights[0].linear", 0.09f);
    renderSystem.SetUniform("pointLights[0].quadratic", 0.032f);
    // point light 2
    renderSystem.SetUniform("pointLights[1].position", pointLightPositions[1]);
    renderSystem.SetUniform("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    renderSystem.SetUniform("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    renderSystem.SetUniform("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    renderSystem.SetUniform("pointLights[1].constant", 1.0f);
    renderSystem.SetUniform("pointLights[1].linear", 0.09f);
    renderSystem.SetUniform("pointLights[1].quadratic", 0.032f);
    // point light 3
    renderSystem.SetUniform("pointLights[2].position", pointLightPositions[2]);
    renderSystem.SetUniform("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    renderSystem.SetUniform("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    renderSystem.SetUniform("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    renderSystem.SetUniform("pointLights[2].constant", 1.0f);
    renderSystem.SetUniform("pointLights[2].linear", 0.09f);
    renderSystem.SetUniform("pointLights[2].quadratic", 0.032f);
    // point light 4
    renderSystem.SetUniform("pointLights[3].position", pointLightPositions[3]);
    renderSystem.SetUniform("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    renderSystem.SetUniform("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    renderSystem.SetUniform("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    renderSystem.SetUniform("pointLights[3].constant", 1.0f);
    renderSystem.SetUniform("pointLights[3].linear", 0.09f);
    renderSystem.SetUniform("pointLights[3].quadratic", 0.032f);
    // spotLight
    renderSystem.SetUniform("spotLight.position", m_camera.position);
    renderSystem.SetUniform("spotLight.direction", m_camera.GetNormalizedViewVector()); // TODO: проверить
    renderSystem.SetUniform("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    renderSystem.SetUniform("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    renderSystem.SetUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    renderSystem.SetUniform("spotLight.constant", 1.0f);
    renderSystem.SetUniform("spotLight.linear", 0.09f);
    renderSystem.SetUniform("spotLight.quadratic", 0.032f);
    renderSystem.SetUniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    renderSystem.SetUniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // view/projection transformations
    renderSystem.SetUniform("projection", m_perspective);
    renderSystem.SetUniform("view", m_camera.GetViewMatrix());

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    renderSystem.SetUniform("model", model);

    renderSystem.Bind(m_diffuseMap, 0);
    renderSystem.Bind(m_specularMap, 1);

    // render containers
    for (unsigned int i = 0; i < 10; i++)
    {
        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * (float)i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        renderSystem.SetUniform("model", model);

        renderSystem.Draw(m_cube->vao);
    }
}
//-----------------------------------------------------------------------------
void _013OtherMultipleLights::Update(float deltaTime)
{
    if (GetInput().IsKeyDown(Input::KEY_ESCAPE))
    {
        ExitRequest();
        return;
    }

    const float mouseSensitivity = 10.0f * deltaTime;
    const float moveSpeed = 10.0f * deltaTime;
    const glm::vec3 oldCameraPos = m_camera.position;

    if (GetInput().IsKeyDown(Input::KEY_W)) m_camera.MoveBy(moveSpeed);
    if (GetInput().IsKeyDown(Input::KEY_S)) m_camera.MoveBy(-moveSpeed);
    if (GetInput().IsKeyDown(Input::KEY_A)) m_camera.StrafeBy(moveSpeed);
    if (GetInput().IsKeyDown(Input::KEY_D)) m_camera.StrafeBy(-moveSpeed);

    glm::vec2 delta = GetInput().GetMouseDeltaPosition();
    if (delta.x != 0.0f)  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
    if (delta.y != 0.0f)  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------