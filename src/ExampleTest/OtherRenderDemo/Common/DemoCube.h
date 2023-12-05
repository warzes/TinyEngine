#pragma once

inline GeometryBufferRef CreateDemoCube(ShaderProgramRef shader)
{
	glm::vec3 vpoint[] =
	{
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },

		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },

		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },

		{ 0.5f,  0.5f,  0.5f },
		{ 0.5f,  0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f,  0.5f },
		{ 0.5f,  0.5f,  0.5f },

		{-0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f,  0.5f },
		{ 0.5f, -0.5f,  0.5f },
		{-0.5f, -0.5f,  0.5f },
		{-0.5f, -0.5f, -0.5f },

		{-0.5f,  0.5f, -0.5f },
		{ 0.5f,  0.5f, -0.5f },
		{ 0.5f,  0.5f,  0.5f },
		{ 0.5f,  0.5f,  0.5f },
		{-0.5f,  0.5f,  0.5f },
		{-0.5f,  0.5f, -0.5f },
	};

	return GetRenderSystem().CreateGeometryBuffer(BufferUsage::StaticDraw,
		static_cast<unsigned>(Countof(vpoint)),
		static_cast<unsigned>(sizeof(glm::vec3)),
		vpoint,
		shader);
}

inline ShaderProgramRef CreateDemoCubeShader()
{
	constexpr const char* vs = R"(
layout(location = 0) in vec3 aPosition;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out float Red;
out float Green;
out float Blue;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(aPosition, 1.0);
	Red = Green = Blue = 0.0;
	if(gl_VertexID == 0){
		Red = 1.0;
	}
	else if(gl_VertexID == 1){
		Green = 1.0;
	}
	else if(gl_VertexID == 2){
		Blue = 1.0;
	}
}
)";

	constexpr const char* fs = R"(
in float Red;
in float Green;
in float Blue;

out vec4 FragmentColor;

const vec3 COLOR[6] = vec3[](
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 1.0, 0.0));

void main()
{
	FragmentColor= vec4(COLOR[gl_PrimitiveID / 2], 1.0);
}
)";

	return GetRenderSystem().CreateShaderProgram({ vs }, { fs });
}