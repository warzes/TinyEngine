#include "stdafx.h"
#include "RenderSystem.h"
#include "OpenGLTranslateToGL.h"
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
VertexArrayRef RenderSystem::CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, ShaderProgramRef shaders)
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
void RenderSystem::UpdateBuffer(GPUBufferRef buffer, unsigned offset, unsigned count, unsigned size, const void* data)
{
	assert(IsValid(buffer));

	// TODO: оптимизировать

	// это вершинный буффер?
	if (buffer->type == BufferTarget::ArrayBuffer)
		Bind(buffer->parentArray);

	const GLenum target = TranslateToGL(buffer->type);
	const unsigned id = *buffer;
	const unsigned cacheId = getCurrentCacheBufferFromType(buffer->type);

	if (cacheId != id) glBindBuffer(target, id);

	if (offset == 0 && (buffer->count != count || buffer->size != size))
		glBufferData(target, count * size, data, TranslateToGL(buffer->usage));
	else
		glBufferSubData(target, offset, count * size, data);

	buffer->count = count;
	buffer->size = size;

	// restore current buffer
	//if( cacheId != id ) glBindBuffer(target, cacheId);
}
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
void* RenderSystem::MapBuffer(GPUBufferRef buffer)
{
	assert(IsValid(buffer));
	Bind(buffer);
	return glMapBuffer(TranslateToGL(buffer->type), GL_WRITE_ONLY);
}
#endif
//-----------------------------------------------------------------------------
void* RenderSystem::MapBuffer(GPUBufferRef buffer, unsigned offset, unsigned size)
{
	// TODO: нужно подключать вао
	assert(IsValid(buffer));
	Bind(buffer);
	return glMapBufferRange(TranslateToGL(buffer->type), offset, size, GL_MAP_WRITE_BIT);
}
//-----------------------------------------------------------------------------
bool RenderSystem::UnmapBuffer(GPUBufferRef buffer)
{
	// TODO: нужно подключать вао
	assert(IsValid(buffer));
	assert(*buffer == getCurrentCacheBufferFromType(buffer->type));
	return GL_TRUE == glUnmapBuffer(TranslateToGL(buffer->type));
}
//-----------------------------------------------------------------------------