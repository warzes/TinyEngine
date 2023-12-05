#pragma once

inline GeometryBufferRef CreateDemoPlane(ShaderProgramRef shader)
{
	glm::vec3 vpoint[] =
	{
		{-1.0f, 0.0f,  1.0f },
		{ 1.0f, 0.0f,  1.0f },
		{ 1.0f, 0.0f, -1.0f },
		{-1.0f, 0.0f, -1.0f }
	};

	unsigned vpoint_index[] = {
		0, 1, 2,
		0, 2, 3
	};

	return GetRenderSystem().CreateGeometryBuffer(BufferUsage::StaticDraw,
		static_cast<unsigned>(Countof(vpoint)),
		static_cast<unsigned>(sizeof(glm::vec3)),
		vpoint,
		static_cast<unsigned>(Countof(vpoint_index)),
		IndexFormat::UInt32,
		vpoint_index,
		shader);
}

inline ShaderProgramRef CreateDemoPlaneShader()
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

void main()
{
	FragmentColor = vec4(Red, Green, Blue, 1.0);
}
)";

	return GetRenderSystem().CreateShaderProgram({vs}, {fs});
}