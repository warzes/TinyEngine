#include "stdafx.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"
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
GPUBufferRef RenderSystem::CreateVertexBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	GPUBufferRef resource(new GPUBuffer(BufferType::ArrayBuffer, usage, vertexCount, vertexSize));
	if (!IsValid(resource))
	{
		LogError("VertexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ARRAY_BUFFER, *resource);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current vb
	return resource;
}
//-----------------------------------------------------------------------------
GPUBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexFormat indexFormat, const void* data)
{
	const unsigned indexSize = SizeIndexType(indexFormat);
	GPUBufferRef resource(new GPUBuffer(BufferType::ElementArrayBuffer, usage, indexCount, indexSize));
	if (!IsValid(resource))
	{
		LogError("IndexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *resource);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current ib
	return resource;
}
//-----------------------------------------------------------------------------
VertexArrayRef RenderSystem::CreateVertexArray(GPUBufferRef vbo, GPUBufferRef ibo, const std::vector<VertexAttribute>& attribs)
{
	if (vbo == nullptr || attribs.size() == 0)
	{
		LogError("VertexBuffer is null");
		return {};
	}
	if (vbo->type != BufferType::ArrayBuffer)
	{
		LogError("vbo is not VertexBuffer valid!");
		return {};
	}
	if (ibo && ibo->type != BufferType::ElementArrayBuffer)
	{
		LogError("шbo is not IndexBuffer valid!");
		return {};
	}

	VertexArrayRef resource(new VertexArray(vbo, ibo, static_cast<unsigned>(attribs.size())));
	if (!IsValid(resource))
	{
		LogError("VertexArray create failed!");
		return {};
	}
	vbo->parentArray = resource;
	if (ibo) ibo->parentArray = resource;

	glBindVertexArray(*resource);
	Bind(resource->vbo);
	for (size_t i = 0; i < attribs.size(); i++)
	{
		Bind(attribs[i]);
	}
	Bind(resource->ibo);

	glBindVertexArray(m_cache.CurrentVAO); // restore VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current IBO

	return resource;
}
//-----------------------------------------------------------------------------
VertexArrayRef RenderSystem::CreateVertexArray(GPUBufferRef vbo, GPUBufferRef ibo, ShaderProgramRef shaders)
{
	auto attribInfo = GetAttributesInfo(shaders);
	if (attribInfo.empty()) return {};

	size_t offset = 0;
	std::vector<VertexAttribute> attribs(attribInfo.size());
	for (size_t i = 0; i < attribInfo.size(); i++)
	{
		// TODO: gl_VertexID считается атрибутом, но при этом почему-то location у него равен -1
		// в будущем пофиксить, а пока не юзать
		assert(attribInfo[i].location > -1);
		attribs[i].location = (unsigned)attribInfo[i].location; // TODO: attribInfo[i].location может быть -1, тогда как в attribs[i].location unsigned. нужно исправить
		attribs[i].normalized = false;

		size_t sizeType = 0;
		switch (attribInfo[i].typeId)
		{
		case GL_FLOAT:
			attribs[i].size = 1;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC2:
			attribs[i].size = 2;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC3:
			attribs[i].size = 3;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC4:
			attribs[i].size = 4;
			sizeType = attribs[i].size * sizeof(float);
			break;

		default:
			LogError("Shader attribute type: " + attribInfo[i].name + " not support!");
			return {};
		}

		attribs[i].offset = (void*)+offset;
		offset += sizeType;
	}
	for (size_t i = 0; i < attribs.size(); i++)
	{
		attribs[i].stride = (int)offset;
	}

	return CreateVertexArray(vbo, ibo, attribs);
}
//-----------------------------------------------------------------------------