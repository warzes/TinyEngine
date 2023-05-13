#include "stdafx.h"
#include "RenderResource.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"
#include "STBImageLoader.h"
//-----------------------------------------------------------------------------
Shader::Shader(ShaderType type)
{
	m_handle = glCreateShader(TranslateToGL(type));
	m_ownership = true;
}
//-----------------------------------------------------------------------------
bool ShaderSource::LoadFromFile(const std::string& file)
{
	// TODO: возможно заменить fstream на file?
	m_filename = file;
	//m_path = ; не забыть
	std::ifstream shaderFile(file); // TODO: заменить
	if( !shaderFile.is_open() )
	{
		LogError("Shader file '" + file + "' not found.");
		return false;
	}
	std::stringstream shader_string;
	shader_string << shaderFile.rdbuf();
	m_src = shader_string.str();
	// Remove the EOF from the end of the string.
	if( m_src[m_src.length() - 1] == EOF )
		m_src.pop_back();
	return true;
}
//-----------------------------------------------------------------------------
ShaderProgramRef RenderSystem::CreateShaderProgram(const ShaderSource& vertexShaderSource, const ShaderSource& fragmentShaderSource)
{
	if( !vertexShaderSource.IsValid() )
	{
		LogError("You must provide vertex shader (source is blank).");
		return {};
	}

	if( !fragmentShaderSource.IsValid() )
	{
		LogError("You must provide fragment shader (source is blank).");
		return {};
	}

	ShaderRef glShaderVertex   = compileShader(ShaderType::Vertex, vertexShaderSource.GetSource());
	ShaderRef glShaderFragment = compileShader(ShaderType::Fragment, fragmentShaderSource.GetSource());

	ShaderProgramRef resource;
	if( IsValid(glShaderVertex) && IsValid(glShaderFragment) )
	{
		resource.reset(new ShaderProgram());
		glAttachShader(*resource, *glShaderVertex);
		glAttachShader(*resource, *glShaderFragment);
		glLinkProgram(*resource);

		GLint linkStatus = 0;
		glGetProgramiv(*resource, GL_LINK_STATUS, &linkStatus);
		if( linkStatus == GL_FALSE )
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
GPUBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexType indexFormat, const void * data)
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

	glBindVertexArray(*resource);
	Bind(resource->vbo);
	for (size_t i = 0; i < attribs.size(); i++)
	{
		Bind(attribs[i]);
	}
	Bind(resource->ibo);

	glBindVertexArray(m_cache.CurrentVAO); // restore VAO
	glBindVertexArray(m_cache.CurrentVBO); // restore VBO
	glBindVertexArray(m_cache.CurrentIBO); // restore IBO

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
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexType indexFormat, const void* indexData, ShaderProgramRef shaders)
{
	assert(IsValid(shaders));

	GeometryBufferRef geom(new GeometryBuffer());

	GPUBufferRef vb = CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if (!IsValid(vb))
	{
		LogError("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	const bool isIndexBuffer = indexCount > 0;
	GPUBufferRef ib = nullptr;
	if (isIndexBuffer)
	{
		ib = CreateIndexBuffer(usage, indexCount, indexFormat, indexData);
		if (!IsValid(ib))
		{
			LogError("GeometryBuffer::IndexBuffer create failed!!");
			return {};
		}
	}

	geom->vao = CreateVertexArray(vb, ib, shaders);
	if (!IsValid(geom->vao))
	{
		LogError("GeometryBuffer::VertexArray create failed!!");
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexType indexFormat, const void* indexData, const std::vector<VertexAttribute>& attribs)
{
	GeometryBufferRef geom(new GeometryBuffer());

	GPUBufferRef vb = CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if (!IsValid(vb))
	{
		LogError("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	const bool isIndexBuffer = indexCount > 0;
	GPUBufferRef ib = nullptr;
	if (isIndexBuffer)
	{
		ib = CreateIndexBuffer(usage, indexCount, indexFormat, indexData);
		if (!IsValid(ib))
		{
			LogError("GeometryBuffer::IndexBuffer create failed!!");
			return {};
		}
	}

	geom->vao = CreateVertexArray(vb, ib, attribs);
	if (!IsValid(geom->vao))
	{
		LogError("GeometryBuffer::VertexArray create failed!!");
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, ShaderProgramRef shaders)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, {}, nullptr, shaders);
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, {}, nullptr, attribs);
}
//-----------------------------------------------------------------------------
TexelsFormat Convert(ImagePixelFormat format)
{
	switch (format)
	{
	case ImagePixelFormat::R_U8: return TexelsFormat::R_U8;
	case ImagePixelFormat::RG_U8: return TexelsFormat::RG_U8;
	case ImagePixelFormat::RGB_U8: return TexelsFormat::RGB_U8;
	case ImagePixelFormat::RGBA_U8: return TexelsFormat::RGBA_U8;
	case ImagePixelFormat::None:
	default: return TexelsFormat::None;
	}
}
//-----------------------------------------------------------------------------
Texture2DRef RenderSystem::CreateTexture2D(const char* fileName, bool useCache, const Texture2DInfo& textureInfo)
{
	if( useCache )
	{
		auto it = m_cacheFileTextures2D.find(fileName);
		if( it != m_cacheFileTextures2D.end() )
			return it->second;
	}

	LogPrint("Load texture: " + std::string(fileName));

	STBImageLoader imageLoad(fileName);
	if( imageLoad.pixelData == nullptr )
	{
		LogError("Image loading failed! Filename='" + std::string(fileName) + "'");
		return {};
	}
	const Texture2DCreateInfo createInfo = {
		.format = Convert(imageLoad.imageFormat),
		.width = static_cast<uint16_t>(imageLoad.width),
		.height = static_cast<uint16_t>(imageLoad.height),
		.pixelData = imageLoad.pixelData,
	};

	m_cacheFileTextures2D[fileName] = CreateTexture2D(createInfo, textureInfo);
	return m_cacheFileTextures2D[fileName];
}
//-----------------------------------------------------------------------------
Texture2DRef RenderSystem::CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo)
{
	Texture2DRef resource(new Texture2D(createInfo.width, createInfo.height, createInfo.format));
	// gen texture res
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *resource);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TranslateToGL(textureInfo.wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TranslateToGL(textureInfo.wrapT));

	// set texture filtering parameters
	TextureMinFilter minFilter = textureInfo.minFilter;
	if( !textureInfo.mipmap )
	{
		if( textureInfo.minFilter == TextureMinFilter::NearestMipmapNearest ) minFilter = TextureMinFilter::Nearest;
		else if( textureInfo.minFilter != TextureMinFilter::Nearest ) minFilter = TextureMinFilter::Linear;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TranslateToGL(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TranslateToGL(textureInfo.magFilter));

	// set texture format
	GLenum format = GL_RGB;
	GLint internalFormat = GL_RGB;
	GLenum oglType = GL_UNSIGNED_BYTE;
	if( !GetTextureFormatType(createInfo.format, GL_TEXTURE_2D, format, internalFormat, oglType) )
	{
		resource.reset();
		glBindTexture(GL_TEXTURE_2D, m_cache.CurrentTexture2D[0]);
		return {};
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (GLsizei)resource->width, (GLsizei)resource->height, 0, format, oglType, createInfo.pixelData);

	if( textureInfo.mipmap )
		glGenerateMipmap(GL_TEXTURE_2D);

	// restore prev state
	glBindTexture(GL_TEXTURE_2D, m_cache.CurrentTexture2D[0]);

	return resource;
}
//-----------------------------------------------------------------------------
RenderbufferRef RenderSystem::CreateRenderbuffer(const glm::uvec2& size, ImageFormat format, int multisample)
{
	assert(size.x > 0 && size.y > 0);
	if( multisample < 1 ) multisample = 1;

	RenderbufferRef resource(new Renderbuffer());
	resource->size = size;
	resource->multisample = multisample;
	resource->format = format;

	glBindRenderbuffer(GL_RENDERBUFFER, *resource);
	if( multisample > 1 )
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, TranslateToGL(format), static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
	}
	else
		glRenderbufferStorage(GL_RENDERBUFFER, TranslateToGL(format), static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return resource;
}
//-----------------------------------------------------------------------------
FramebufferRef RenderSystem::CreateFramebuffer(RenderbufferRef colorBuffer)
{
	FramebufferRef resource(new Framebuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	attachmentFrameBufferColor(resource, colorBuffer);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	if( !checkCurrentFrameBuffer() )
		resource.reset();
	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
FramebufferRef RenderSystem::CreateFramebuffer(RenderbufferRef colorBuffer, RenderbufferRef depthStencilBuffer)
{
	FramebufferRef resource(new Framebuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	attachmentFrameBufferColor(resource, colorBuffer);
	attachmentFrameBufferDepthStencil(resource, depthStencilBuffer);
	if( !checkCurrentFrameBuffer() )
		resource.reset();
	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
FramebufferRef RenderSystem::CreateFramebuffer(Texture2DRef colorTexture)
{
	FramebufferRef resource(new Framebuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	attachmentFrameBufferColor(resource, colorTexture);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	if( !checkCurrentFrameBuffer() )
		resource.reset();
	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
FramebufferRef RenderSystem::CreateFramebuffer(Texture2DRef colorTexture, RenderbufferRef depthStencilBuffer)
{
	FramebufferRef resource(new Framebuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	attachmentFrameBufferColor(resource, colorTexture);
	attachmentFrameBufferDepthStencil(resource, depthStencilBuffer);
	if( !checkCurrentFrameBuffer() )
		resource.reset();
	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
FramebufferRef RenderSystem::CreateFramebuffer(Texture2DRef colorTexture, Texture2DRef depthStencilTexture)
{
	FramebufferRef resource(new Framebuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	attachmentFrameBufferColor(resource, colorTexture);
	attachmentFrameBufferDepthStencil(resource, depthStencilTexture);
	if( !checkCurrentFrameBuffer() )
		resource.reset();

	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
FramebufferRef RenderSystem::CreateFramebuffer(FramebufferAttachment attachment, Texture2DRef texture)
{
	assert(IsValid(texture));

	FramebufferRef resource(new Framebuffer());
	resource->colorTexture = texture;
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	glFramebufferTexture2D(GL_FRAMEBUFFER, TranslateToGL(attachment), GL_TEXTURE_2D, *resource->colorTexture, 0);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( GL_FRAMEBUFFER_COMPLETE != result )
	{
		LogError("Framebuffer is not complete.");
		return {};
	}
	// restore prev state
	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
ShaderRef RenderSystem::compileShader(ShaderType type, const std::string& source)
{
	const char* shaderText = source.data();
	const GLint lenShaderText = static_cast<GLint>(source.size());

	ShaderRef shader(new Shader(type));
	glShaderSource(*shader, 1, &shaderText, &lenShaderText);
	glCompileShader(*shader);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if( compileStatus == GL_FALSE )
	{
		GLint infoLogLength;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::unique_ptr<GLchar> errorInfoText{ new GLchar[static_cast<size_t>(infoLogLength + 1)] };
		glGetShaderInfoLog(*shader, infoLogLength, nullptr, errorInfoText.get());

		const std::string shaderName = ConvertToStr(type);
		LogError(shaderName + " Shader compilation failed : " + std::string(errorInfoText.get()) + ", Source: " + source);
		return {};
	}

	return shader;
}
//-----------------------------------------------------------------------------
void RenderSystem::attachmentFrameBufferColor(FramebufferRef fbo, RenderbufferRef colorBuffer)
{
	if( colorBuffer )
	{
		fbo->size = colorBuffer->size;
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, *colorBuffer);
		fbo->colorBuffer = colorBuffer;
	}
	else
	{
		glDrawBuffer(GL_NONE);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::attachmentFrameBufferColor(FramebufferRef fbo, Texture2DRef colorTexture)
{
	if( colorTexture )
	{
		fbo->size = { colorTexture->width, colorTexture->height };
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorTexture, 0);
		fbo->colorTexture = colorTexture;
	}
	else
	{
		glDrawBuffer(GL_NONE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::attachmentFrameBufferDepthStencil(FramebufferRef fbo, RenderbufferRef depthStencilBuffer)
{
	if( depthStencilBuffer )
	{
		if( fbo->size != glm::uvec2(0) && fbo->size != depthStencilBuffer->size )
			LogWarning("Framebuffer color and depth dimensions don't match");
		fbo->size = depthStencilBuffer->size;
		fbo->depthStencilBuffer = depthStencilBuffer;

		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depthStencilBuffer);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer->format == ImageFormat::D24S8 ? *depthStencilBuffer : 0u);

	}
	else
	{
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::attachmentFrameBufferDepthStencil(FramebufferRef fbo, Texture2DRef depthStencilTexture)
{
	if( depthStencilTexture )
	{
		if( fbo->size != glm::uvec2(0) && fbo->size != glm::uvec2({ depthStencilTexture->width, depthStencilTexture->height }) )
			LogWarning("Framebuffer color and depth dimensions don't match");
		fbo->size = { depthStencilTexture->width, depthStencilTexture->height };
		fbo->depthStencilTexture = depthStencilTexture;

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthStencilTexture, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->format == TexelsFormat::DepthStencil_U24 ? *depthStencilTexture : 0u, 0);
	}
	else
	{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}
}
//-----------------------------------------------------------------------------