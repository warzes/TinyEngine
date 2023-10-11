#include "stdafx.h"
#include "RenderResource.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"
#include "Core/IO/ImageLoader.h"
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(Uniform) == 8, "Uniform size changed!!!");
static_assert(sizeof(glObject) == 8, "glObject size changed!!!");
static_assert(sizeof(Shader) == 12, "Shader size changed!!!");
static_assert(sizeof(ShaderProgram) == 8, "ShaderProgram size changed!!!");
static_assert(sizeof(GPUBuffer) == 40, "GPUBuffer size changed!!!");
static_assert(sizeof(VertexArray) == 48, "VertexArray size changed!!!");
static_assert(sizeof(GeometryBuffer) == 16, "GeometryBuffer size changed!!!");
static_assert(sizeof(Texture2D) == 20, "Texture2D size changed!!!");
static_assert(sizeof(Renderbuffer) == 24, "Renderbuffer size changed!!!");
static_assert(sizeof(Framebuffer) == 96, "Framebuffer size changed!!!");
#	if USE_OPENGL_VERSION == OPENGL40
static_assert(sizeof(TransformFeedback) == 8, "TransformFeedback size changed!!!");
#	endif // OPENGL40
#endif // PLATFORM_EMSCRIPTEN


//-----------------------------------------------------------------------------
ShaderBytecode::ShaderBytecode(ShaderSourceType shaderSource, const std::string& text)
{
	m_shaderSource = shaderSource;
	if (m_shaderSource == ShaderSourceType::CodeString)
		m_src = text;
	else if (m_shaderSource == ShaderSourceType::CodeFile)
	{
		m_src = text;
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
bool ShaderBytecode::LoadFromFile(const std::string & file)
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
	m_src = shader_string.str();
	// Remove the EOF from the end of the string.
	if (m_src[m_src.length() - 1] == EOF)
		m_src.pop_back();
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
Shader::Shader(ShaderPipelineStage stage)
{
	m_handle = glCreateShader(TranslateToGL(stage));
	m_ownership = true;
	shaderStage = stage;
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexFormat indexFormat, const void* indexData, ShaderProgramRef shaders)
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
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexFormat indexFormat, const void* indexData, const std::vector<VertexAttribute>& attribs)
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
TexelsFormat Convert(ImageLoader::ImageFormat format)
{
	switch (format)
	{
	case ImageLoader::R_U8:    return TexelsFormat::R_U8;
	case ImageLoader::RG_U8:   return TexelsFormat::RG_U8;
	case ImageLoader::RGB_U8:  return TexelsFormat::RGB_U8;
	case ImageLoader::RGBA_U8: return TexelsFormat::RGBA_U8;
	case ImageLoader::None:
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

	ImageLoader imageLoad(fileName);
	auto* pixelData = imageLoad.GetTexels();
	if( pixelData == nullptr )
	{
		LogError("Image loading failed! Filename='" + std::string(fileName) + "'");
		return {};
	}
	const Texture2DCreateInfo createInfo = {
		.format = Convert(imageLoad.GetImageFormat()),
		.width = static_cast<uint16_t>(imageLoad.GetWidth()),
		.height = static_cast<uint16_t>(imageLoad.GetHeight()),
		.pixelData = pixelData,
		.hasTransparency = imageLoad.HasTransparency()
	};

	m_cacheFileTextures2D[fileName] = CreateTexture2D(createInfo, textureInfo);
	return m_cacheFileTextures2D[fileName];
}
//-----------------------------------------------------------------------------
Texture2DRef RenderSystem::CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo)
{
	Texture2DRef resource(new Texture2D(createInfo.width, createInfo.height, createInfo.format));
	resource->hasTransparency = createInfo.hasTransparency;
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
FramebufferRef RenderSystem::CreateFramebuffer(const std::vector<Texture2DRef>& colorTextures, RenderbufferRef depthStencilBuffer)
{
	FramebufferRef resource(new Framebuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	attachmentFrameBufferColor(resource, colorTextures);
	attachmentFrameBufferDepthStencil(resource, depthStencilBuffer);
	if (!checkCurrentFrameBuffer())
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
	resource->colorTextures[0] = texture;
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	glFramebufferTexture2D(GL_FRAMEBUFFER, TranslateToGL(attachment), GL_TEXTURE_2D, *resource->colorTextures[0], 0);
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
ShaderRef RenderSystem::compileShader(ShaderPipelineStage type, const std::string& source)
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
#if !PLATFORM_EMSCRIPTEN
		glDrawBuffer(GL_COLOR_ATTACHMENT0); // TODO: нужно ли?
#endif
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, *colorBuffer);
		fbo->colorBuffer = colorBuffer;
	}
	else
	{
#if !PLATFORM_EMSCRIPTEN
		glDrawBuffer(GL_NONE); // TODO: нужно ли?
#endif
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::attachmentFrameBufferColor(FramebufferRef fbo, Texture2DRef colorTexture)
{
	if( colorTexture )
	{
		fbo->size = { colorTexture->width, colorTexture->height };
#if !PLATFORM_EMSCRIPTEN
		glDrawBuffer(GL_COLOR_ATTACHMENT0); // TODO: нужно ли?
#endif
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorTexture, 0);
		fbo->colorTextures[0] = colorTexture;
	}
	else
	{
#if !PLATFORM_EMSCRIPTEN
		glDrawBuffer(GL_NONE); // TODO: нужно ли?
#endif
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::attachmentFrameBufferColor(FramebufferRef fbo, const std::vector<Texture2DRef>& colorTextures)
{
	if (colorTextures.empty() || !colorTextures[0])
	{
#if !PLATFORM_EMSCRIPTEN
		glDrawBuffer(GL_NONE); // TODO: нужно ли?
#endif
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	}
	else
	{
		std::vector<uint32_t> colorAttachments;
		for (size_t i = 0; i < colorTextures.size(); i++)
		{
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *colorTextures[i], 0);
			colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(static_cast<GLsizei>(colorAttachments.size()), colorAttachments.data());
		fbo->size = { colorTextures[0]->width, colorTextures[0]->height }; // TODO: нужно как-то разобраться с ситуацией если текстуры разных размеров - скорее всго это ошибка, но надо обработать
		fbo->colorTextures = colorTextures;
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
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *depthStencilBuffer);

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