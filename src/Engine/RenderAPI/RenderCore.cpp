#include "stdafx.h"
#include "RenderCore.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
ShaderBytecode::ShaderBytecode(ShaderSourceType shaderSource, const std::string& text)
{
	m_shaderSource = shaderSource;
	if (m_shaderSource == ShaderSourceType::CodeMemory)
		m_sourceCode = text;
	else if (m_shaderSource == ShaderSourceType::CodeFile)
	{
		m_sourceCode = text;
		LoadFromFile(text);
	}
	else if (m_shaderSource == ShaderSourceType::BinaryBuffer)
	{
		LogFatal("Not impl!");
	}
	else if (m_shaderSource == ShaderSourceType::BinaryFile)
	{
		LogFatal("Not impl!");
	}
}
//-----------------------------------------------------------------------------
bool ShaderBytecode::LoadFromFile(const std::string& file)
{
	m_shaderSource = ShaderSourceType::CodeFile;

	// TODO: возможно заменить fstream на file?
	m_filename = file;
	//m_path = ; не забыть
	std::ifstream shaderFile(file); // TODO: заменить
	if (!shaderFile.is_open())
	{
		LogError("Shader file '" + file + "' not found.");
		return false;
	}
	std::stringstream shader_string;
	shader_string << shaderFile.rdbuf();
	m_sourceCode = shader_string.str();
	// Remove the EOF from the end of the string.
	if (m_sourceCode[m_sourceCode.length() - 1] == EOF)
		m_sourceCode.pop_back();
	return true;
}
//-----------------------------------------------------------------------------
std::string ShaderBytecode::GetHeaderVertexShader()
{
#if PLATFORM_EMSCRIPTEN
	return "#version 300 es";
#else
	return R"(
#version 330 core
)";
#endif
}
//-----------------------------------------------------------------------------
std::string ShaderBytecode::GetHeaderFragmentShader()
{
#if PLATFORM_EMSCRIPTEN
	return R"(#version 300 es
precision mediump float;

)";
#else
	return R"(
#version 330 core
)";
#endif
}
//-----------------------------------------------------------------------------