#include "stdafx.h"
#include "RenderSystem.h"
#include "OpenGLTranslateToGL.h"
//-----------------------------------------------------------------------------
[[nodiscard]] inline GLenum getAttributeType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
		return GL_FLOAT;
	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return GL_INT;
	}
	assert(false && "Unknown active attribute type!");
	return 0;
}
//-----------------------------------------------------------------------------
[[nodiscard]] inline GLint getAttributeSize(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
	case GL_INT:
		return 1;
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
		return 2;
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
		return 3;
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
		return 4;
	}

	assert(false && "Unknown active attribute type!");
	return 0;
}
//-----------------------------------------------------------------------------
ShaderProgramRef RenderSystem::CreateShaderProgram(const ShaderBytecode& vertexShaderSource, const ShaderBytecode& fragmentShaderSource)
{
	if (!vertexShaderSource.IsValid())
	{
		LogError("You must provide vertex shader (source is blank).");
		return nullptr;
	}

	if (!fragmentShaderSource.IsValid())
	{
		LogError("You must provide fragment shader (source is blank).");
		return nullptr;
	}

	std::string vertexShaderSourceCode = vertexShaderSource.GetSource();
	std::string fragmentShaderSourceCode = fragmentShaderSource.GetSource();
	validationShaderCode(vertexShaderSourceCode, fragmentShaderSourceCode);

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
	glShaderVertex.reset();
	glShaderFragment.reset();

	return resource;
}
//-----------------------------------------------------------------------------
bool RenderSystem::IsReadyUniform(const Uniform& uniform) const
{
	return IsValid(uniform) && uniform.programId == m_cache.CurrentShaderProgram;
}
//-----------------------------------------------------------------------------
std::vector<ShaderAttributeInfo> RenderSystem::GetAttributesInfo(ShaderProgramRef program) const
{
	assert(IsValid(program));
	if (!IsValid(program)) return {};

	int activeAttribsCount = 0;
	glGetProgramiv(*program, GL_ACTIVE_ATTRIBUTES, &activeAttribsCount);
	int maxNameLength = 0;
	glGetProgramiv(*program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

	std::string name;
	name.resize(static_cast<size_t>(maxNameLength));

	std::vector<ShaderAttributeInfo> attribs;
	for (size_t i = 0; i < static_cast<size_t>(activeAttribsCount); i++)
	{
		GLint size;
		GLenum type = 0;
		glGetActiveAttrib(*program, (GLuint)i, maxNameLength, nullptr, &size, &type, name.data());

		std::size_t pos = name.find("gl_VertexID"); // выкидается из шейдера так как location = -1
		if (pos != std::string::npos) continue;

		int location = glGetAttribLocation(*program, name.c_str());
		assert(location >= 0);

		attribs.emplace_back(ShaderAttributeInfo{
			.typeId = type,
			.type = getAttributeType(type),
			.numType = getAttributeSize(type),
			.name = name,
			.location = location
		});
	}

	std::sort(attribs.begin(), attribs.end(), [](const ShaderAttributeInfo& a, const ShaderAttributeInfo& b) {return a.location < b.location; });

	return attribs;
}
//-----------------------------------------------------------------------------
Uniform RenderSystem::GetUniform(ShaderProgramRef program, const char* uniformName) const
{
	if (!IsValid(program) || uniformName == nullptr) return {};

	if (m_cache.CurrentShaderProgram != *program) glUseProgram(*program);
	Uniform uniform = {
		.location = glGetUniformLocation(*program, uniformName),
		.programId = *program
	};
	// restore prev shader program
	if (m_cache.CurrentShaderProgram != *program) glUseProgram(m_cache.CurrentShaderProgram);
	return uniform;
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, bool value)
{
	assert(IsReadyUniform(uniform));
	glUniform1i(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, int value)
{
	assert(IsReadyUniform(uniform));
	glUniform1i(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, unsigned int value)
{
	assert(IsReadyUniform(uniform));
	glUniform1ui(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, float value)
{
	assert(IsReadyUniform(uniform));
	glUniform1f(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec2& value)
{
	assert(IsReadyUniform(uniform));
	glUniform2fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec3& value)
{
	assert(IsReadyUniform(uniform));
	glUniform3fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec4& value)
{
	assert(IsReadyUniform(uniform));
	glUniform4fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::mat3& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::mat4& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform3(const Uniform& uniform, unsigned number, float* values)
{
	assert(IsReadyUniform(uniform));
	glUniform3fv(uniform.location, static_cast<GLsizei>(number), values);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform4(const Uniform& uniform, unsigned number, float* values)
{
	assert(IsReadyUniform(uniform));
	glUniform4fv(uniform.location, static_cast<GLsizei>(number), values);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<float> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform1fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<glm::vec2> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform2fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<glm::vec3> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform3fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<glm::vec4> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform4fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, bool value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1i(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, int value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1i(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, unsigned value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1ui(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, float value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1f(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec2& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform2fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec3& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform3fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec4& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform4fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::mat3& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniformMatrix3fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::mat4& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniformMatrix4fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(ShaderProgramRef resource)
{
	assert(IsValid(resource));
	if (m_cache.CurrentShaderProgram == *resource) return;
	m_cache.CurrentShaderProgram = *resource;
	glUseProgram(*resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(unsigned rawShader)
{
	if (m_cache.CurrentShaderProgram == rawShader) return;
	m_cache.CurrentShaderProgram = rawShader;
	glUseProgram(rawShader);
}
//-----------------------------------------------------------------------------
void RenderSystem::validationShaderCode(std::string& vertexCode, std::string& fragmentCode)
{
	//============================================================================
	// Add version
	//============================================================================
	size_t posVS = vertexCode.find("#version");
	if (posVS == std::string::npos)
		vertexCode = ShaderBytecode::GetHeaderVertexShader() + vertexCode;

	size_t posFS = fragmentCode.find("#version");
	if (posFS == std::string::npos)
		fragmentCode = ShaderBytecode::GetHeaderFragmentShader() + fragmentCode;
}
//-----------------------------------------------------------------------------
ShaderRef RenderSystem::compileShader(ShaderPipelineStage type, const std::string& source)
{
	const char* shaderText = source.data();
	const GLint lenShaderText = static_cast<GLint>(source.size());

	ShaderRef shader(new Shader(type));
	glShaderSource(*shader, 1, &shaderText, &lenShaderText);
	glCompileShader(*shader);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::unique_ptr<GLchar> errorInfoText{ new GLchar[static_cast<size_t>(infoLogLength + 1)] };
		glGetShaderInfoLog(*shader, infoLogLength, nullptr, errorInfoText.get());

		const std::string shaderName = ConvertToStr(type);
		LogError(shaderName + " Shader compilation failed : " + std::string(errorInfoText.get()) + ", Source: " + source);
		return nullptr;
	}

	return shader;
}
//-----------------------------------------------------------------------------