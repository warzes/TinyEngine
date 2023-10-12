#include "stdafx.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------
ShaderProgramRef RenderSystem::CreateShaderProgram(const ShaderBytecode& vertexShaderSource, const ShaderBytecode& fragmentShaderSource)
{
	if (!vertexShaderSource.IsValid())
	{
		LogError("You must provide vertex shader (source is blank).");
		return {};
	}

	if (!fragmentShaderSource.IsValid())
	{
		LogError("You must provide fragment shader (source is blank).");
		return {};
	}

	std::string vertexShaderSourceCode = vertexShaderSource.GetSource();
	std::string fragmentShaderSourceCode = fragmentShaderSource.GetSource();
	{
		size_t posVS = vertexShaderSourceCode.find("#version");
		if (posVS == std::string::npos)
			vertexShaderSourceCode = ShaderBytecode::GetHeaderVertexShader() + vertexShaderSourceCode;

		size_t posFS = fragmentShaderSourceCode.find("#version");
		if (posFS == std::string::npos)
			fragmentShaderSourceCode = ShaderBytecode::GetHeaderFragmentShader() + fragmentShaderSourceCode;
	}

	ShaderRef glShaderVertex = compileShader(ShaderPipelineStage::Vertex, vertexShaderSourceCode);
	ShaderRef glShaderFragment = compileShader(ShaderPipelineStage::Fragment, fragmentShaderSourceCode);

	ShaderProgramRef resource;
	if (IsValid(glShaderVertex) && IsValid(glShaderFragment))
	{
		resource.reset(new ShaderProgram());

		glAttachShader(*resource, *glShaderVertex);
		glAttachShader(*resource, *glShaderFragment);
		glLinkProgram(*resource);
		GLint linkStatus = 0;
		glGetProgramiv(*resource, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			GLint infoLogLength;
			glGetProgramiv(*resource, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::unique_ptr<GLchar> errorInfoText{ new GLchar[static_cast<size_t>(infoLogLength + 1)] };
			glGetProgramInfoLog(*resource, infoLogLength, nullptr, errorInfoText.get());
			LogError("OPENGL: Shader program linking failed: " + std::string(errorInfoText.get()));
			resource.reset();
		}
		glDetachShader(*resource, *glShaderVertex);
		glDetachShader(*resource, *glShaderFragment);
	}

	return resource;
}
//-----------------------------------------------------------------------------