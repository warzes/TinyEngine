#include "stdafx.h"
#include "RenderSystem.h"
#include "OpenGLTranslateToGL.h"
#include "OpenGLExtensions.h"
//-----------------------------------------------------------------------------
VertexBufferRef RenderSystem::CreateVertexBuffer(BufferUsage usage)
{
	VertexBufferRef resource(new VertexBuffer(usage));
	if (!IsValid(resource))
	{
		LogError("VertexBuffer create failed!");
		return {};
	}
	return resource;
}
//-----------------------------------------------------------------------------
VertexBufferRef RenderSystem::CreateVertexBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	VertexBufferRef resource(new VertexBuffer(usage, vertexCount, vertexSize));
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
IndexBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage)
{
	IndexBufferRef resource(new IndexBuffer(usage));
	if (!IsValid(resource))
	{
		LogError("IndexBuffer create failed!");
		return {};
	}
	return resource;
}
//-----------------------------------------------------------------------------
IndexBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexFormat indexFormat, const void* data)
{
	const unsigned indexSize = SizeIndexType(indexFormat);
	IndexBufferRef resource(new IndexBuffer(usage, indexCount, indexSize));
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
VertexArrayRef RenderSystem::CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, const std::vector<VertexAttribute>& attribs)
{
	if (vbo == nullptr || attribs.size() == 0)
	{
		LogError("VertexBuffer is null");
		return {};
	}
	if (vbo->type != BufferTarget::ArrayBuffer)
	{
		LogError("vbo is not VertexBuffer valid!");
		return {};
	}
	if (ibo && ibo->type != BufferTarget::ElementArrayBuffer)
	{
		LogError("ibo is not IndexBuffer valid!");
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
VertexArrayRef RenderSystem::CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, ShaderProgramRef shaders)
{
	auto attribInfo = GetAttributesInfo(shaders);
	if (attribInfo.empty()) return {};

	size_t offset = 0;
	std::vector<VertexAttribute> attribs(attribInfo.size());
	for (size_t i = 0; i < attribInfo.size(); i++)
	{
		// TODO: gl_VertexID ��������� ���������, �� ��� ���� ������-�� location � ���� ����� -1
		// � ������� ���������, � ���� �� �����
		assert(attribInfo[i].location > -1);
		attribs[i].location = (unsigned)attribInfo[i].location; // TODO: attribInfo[i].location ����� ���� -1, ����� ��� � attribs[i].location unsigned. ����� ���������
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
void RenderSystem::UpdateBuffer(VertexBufferRef buffer, unsigned offset, unsigned count, unsigned size, const void* data)
{
	assert(IsValid(buffer));

	const unsigned id = *buffer;
	const bool isNewBufferData = (offset == 0 && (buffer->count != count || buffer->size != size));
	const GLsizeiptr numberOfBytes = count * size;
	const GLenum glBufferUsage = TranslateToGL(buffer->usage);
	buffer->count = count;
	buffer->size = size;

#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
	{
		if (isNewBufferData) glNamedBufferData(id, numberOfBytes, data, glBufferUsage);
		else glNamedBufferSubData(id, offset, numberOfBytes, data);
	}
	else
#endif
	{
		bool restorePrevState = false;
		if (m_cache.CurrentVAO != *buffer->parentArray || m_cache.CurrentVBO != id)
		{
			glBindVertexArray(*buffer->parentArray);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			restorePrevState = true;
		}

		if (isNewBufferData) glBufferData(GL_ARRAY_BUFFER, numberOfBytes, data, glBufferUsage);
		else glBufferSubData(GL_ARRAY_BUFFER, offset, numberOfBytes, data);

		// restore current buffer
		if (restorePrevState)
		{
			glBindVertexArray(m_cache.CurrentVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentIBO);
		}
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::UpdateBuffer(IndexBufferRef buffer, unsigned offset, unsigned count, unsigned size, const void* data)
{
	assert(IsValid(buffer));

	const unsigned id = *buffer;
	const bool isNewBufferData = (offset == 0 && (buffer->count != count || buffer->size != size));
	const GLsizeiptr numberOfBytes = count * size;
	const GLenum glBufferUsage = TranslateToGL(buffer->usage);
	buffer->count = count;
	buffer->size = size;

#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
	{
		if (isNewBufferData) glNamedBufferData(id, numberOfBytes, data, glBufferUsage);
		else glNamedBufferSubData(id, offset, numberOfBytes, data);
	}
	else
#endif
	{
		if (m_cache.CurrentIBO != id)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

		if (isNewBufferData) glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfBytes, data, glBufferUsage);
		else glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, numberOfBytes, data);

		// restore current buffer
		if (m_cache.CurrentIBO != id)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO);
	}
}
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
inline void* mapBuffer(unsigned buffer, unsigned currentState, GLenum target, GLenum access)
{
#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
		return glMapNamedBuffer(buffer, access);
	else
#endif
	{
		if (currentState != buffer)
		{
			LogError("Buffer not binding");
			return nullptr;
		}
		return glMapBuffer(target, access);
	}
}
#endif
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
void* RenderSystem::MapBuffer(VertexBufferRef buffer, GLenum access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentVBO, GL_ARRAY_BUFFER, access);
}
#endif
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
void* RenderSystem::MapBuffer(IndexBufferRef buffer, GLenum access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentIBO, GL_ELEMENT_ARRAY_BUFFER, access);
}
#endif
//-----------------------------------------------------------------------------
inline void* mapBuffer(unsigned buffer, unsigned currentState, GLenum target, unsigned offset, unsigned size, GLbitfield access)
{
#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
		return glMapNamedBufferRange(buffer, offset, size, access);
	else
#endif
	{
		if (currentState != buffer)
		{
			LogError("Buffer not binding");
			return nullptr;
		}
		return glMapBufferRange(target, offset, size, access);
	}
}
//-----------------------------------------------------------------------------
void* RenderSystem::MapBuffer(VertexBufferRef buffer, unsigned offset, unsigned size, GLbitfield access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentVBO, GL_ARRAY_BUFFER, offset, size, access);
}
//-----------------------------------------------------------------------------
void* RenderSystem::MapBuffer(IndexBufferRef buffer, unsigned offset, unsigned size, GLbitfield access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentIBO, GL_ELEMENT_ARRAY_BUFFER, offset, size, access);
}
//-----------------------------------------------------------------------------
inline bool unmapBuffer(unsigned buffer, unsigned currentState, GLenum target)
{
	if (currentState != buffer)
	{
		LogError("Buffer not binding");
		return false;
	}
#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
		return GL_TRUE == glUnmapNamedBuffer(buffer);
	else
#endif
		return GL_TRUE == glUnmapBuffer(target);
}
//-----------------------------------------------------------------------------
bool RenderSystem::UnmapBuffer(VertexBufferRef buffer)
{
	assert(IsValid(buffer));
	return unmapBuffer(*buffer, m_cache.CurrentVBO, GL_ARRAY_BUFFER);
}
//-----------------------------------------------------------------------------
bool RenderSystem::UnmapBuffer(IndexBufferRef buffer)
{
	assert(IsValid(buffer));
	return unmapBuffer(*buffer, m_cache.CurrentIBO, GL_ELEMENT_ARRAY_BUFFER);
}
//-----------------------------------------------------------------------------