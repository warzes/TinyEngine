#include "stdafx.h"
#include "RenderSystem.h"
#include "OpenGLTranslateToGL.h"
#include "OpenGLCore.h"
//-----------------------------------------------------------------------------
// Use discrete GPU by default.
#if PLATFORM_DESKTOP
extern "C" 
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

	// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif
//-----------------------------------------------------------------------------
#if defined(_DEBUG)
void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*user_param*/) noexcept
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::string msgSource;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             msgSource = "GL_DEBUG_SOURCE_API";             break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: msgSource = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     msgSource = "GL_DEBUG_SOURCE_THIRD_PARTY";     break;
	case GL_DEBUG_SOURCE_APPLICATION:     msgSource = "GL_DEBUG_SOURCE_APPLICATION";     break;
	case GL_DEBUG_SOURCE_OTHER:           msgSource = "GL_DEBUG_SOURCE_OTHER";           break;
	}

	std::string msgType;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               msgType = "GL_DEBUG_TYPE_ERROR";               break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msgType = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  msgType = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";  break;
	case GL_DEBUG_TYPE_PORTABILITY:         msgType = "GL_DEBUG_TYPE_PORTABILITY";         break;
	case GL_DEBUG_TYPE_PERFORMANCE:         msgType = "GL_DEBUG_TYPE_PERFORMANCE";         break;
	case GL_DEBUG_TYPE_OTHER:               msgType = "GL_DEBUG_TYPE_OTHER";               break;
	}

	std::string msgSeverity = "DEFAULT";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:    msgSeverity = "GL_DEBUG_SEVERITY_LOW";    break;
	case GL_DEBUG_SEVERITY_MEDIUM: msgSeverity = "GL_DEBUG_SEVERITY_MEDIUM"; break;
	case GL_DEBUG_SEVERITY_HIGH:   msgSeverity = "GL_DEBUG_SEVERITY_HIGH";   break;
	}

	std::string logMsg = "glDebugMessage: " + std::string(message) + ", type = " + msgType + ", source = " + msgSource + ", severity = " + msgSeverity;

	if (type == GL_DEBUG_TYPE_ERROR) LogWarning(logMsg);
	else                             LogError(logMsg);

}
#endif // _DEBUG
//-----------------------------------------------------------------------------
RenderSystem gRenderSystem;
//-----------------------------------------------------------------------------
bool RenderSystem::Create(const RenderCreateInfo& createInfo)
{
	LogPrint("OpenGL: OpenGL device information:");
	LogPrint("    > Vendor:   " + std::string((const char*)glGetString(GL_VENDOR)));
	LogPrint("    > Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
	LogPrint("    > Version:  " + std::string((const char*)glGetString(GL_VERSION)));
	LogPrint("    > GLSL:     " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

	initializeExtensions(true);
	initializeCapabilities(true);

	// не использовать Bind(state) так как дефолтные значения из кеша могут не соответствовать установкам.

	// set default depth state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// set defautl stensil state
	// TODO: проставить дефолтные значения StencilState

	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(createInfo.clearColor.x, createInfo.clearColor.y, createInfo.clearColor.z, 1.0f);

	setClearMask(true, true, false);

	LogPrint("RenderSystem Create");

	return true;
}
//-----------------------------------------------------------------------------
void RenderSystem::Destroy()
{
	ResetAllStates();
	m_cacheFileTextures2D.clear();
}
//-----------------------------------------------------------------------------
void RenderSystem::SetClearColor(const glm::vec3& color)
{
	glClearColor(color.x, color.y, color.z, 1.0f);
}
//-----------------------------------------------------------------------------
void RenderSystem::ClearFrame()
{
	glClear(m_cache.CurrentClearMask);
}
//-----------------------------------------------------------------------------
void RenderSystem::ClearFrame(const glm::vec3& color)
{
	glClearColor(color.x, color.y, color.z, 1.0f);
	ClearFrame();
}
//-----------------------------------------------------------------------------
void RenderSystem::SetViewport(int width, int height)
{
	SetViewport(0, 0, width, height);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}
//-----------------------------------------------------------------------------
void RenderSystem::MainScreen()
{
	if( m_cache.CurrentFramebuffer > 0 )
	{
		m_cache.CurrentFramebuffer = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// TODO: set current viewport and clear color?
		ClearFrame();
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::ResetAllStates()
{
	m_cache.Reset();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	for (unsigned i = 0; i < MaxBindingTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//-----------------------------------------------------------------------------
void RenderSystem::ResetState(ResourceType type)
{
	if( type == ResourceType::ShaderProgram )
	{
		m_cache.CurrentShaderProgram = 0;
		glUseProgram(0);
	}
	else if( type == ResourceType::VertexBuffer )
	{
		m_cache.CurrentVBO = 0;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if( type == ResourceType::IndexBuffer )
	{
		m_cache.CurrentIBO = 0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else if( type == ResourceType::VertexArray )
	{
		ResetState(ResourceType::VertexBuffer);
		ResetState(ResourceType::IndexBuffer);
		m_cache.CurrentVAO = 0;
		glBindVertexArray(0);
	}
	else if( type == ResourceType::Texture2D )
	{
		for( unsigned i = 0; i < MaxBindingTextures; i++ )
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_cache.CurrentTexture2D[i] = 0;
		}
		glActiveTexture(GL_TEXTURE0);
	}
	else if( type == ResourceType::Framebuffer )
	{
		m_cache.CurrentFramebuffer = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(DepthState state)
{
	if (m_cache.CurrentDepthState.enable != state.enable)
	{
		if (state.enable) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
		setClearMask(true, state.enable, m_cache.CurrentStencilState.enable);
	}

	if (m_cache.CurrentDepthState.compareFunction != state.compareFunction)
		glDepthFunc(TranslateToGL(state.compareFunction));

	if (m_cache.CurrentDepthState.depthWrite != state.depthWrite)
		glDepthMask(static_cast<GLboolean>(state.depthWrite ? GL_TRUE : GL_FALSE));

	m_cache.CurrentDepthState = state;
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(StencilState state)
{
	StencilState& cache = m_cache.CurrentStencilState;
	if (cache.enable != state.enable)
	{
		if (state.enable) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);

		setClearMask(true, m_cache.CurrentDepthState.enable, state.enable);
	}

	if (cache.stencilRef != state.stencilRef 
		|| cache.readMask != state.readMask
		|| cache.frontFaceStencilCompareFunction != state.frontFaceStencilCompareFunction
		|| cache.frontFaceStencilFailureOperation != state.frontFaceStencilFailureOperation
		|| cache.frontFaceStencilDepthFailureOperation != state.frontFaceStencilDepthFailureOperation
		|| cache.frontFaceStencilPassOperation != state.frontFaceStencilPassOperation
		|| cache.backFaceStencilCompareFunction != state.backFaceStencilCompareFunction
		|| cache.backFaceStencilFailureOperation != state.backFaceStencilFailureOperation
		|| cache.backFaceStencilDepthFailureOperation != state.backFaceStencilDepthFailureOperation
		|| cache.backFaceStencilPassOperation != state.backFaceStencilPassOperation)
	{
		glStencilFuncSeparate(GL_FRONT, TranslateToGL(state.frontFaceStencilCompareFunction), state.stencilRef, state.readMask);
		glStencilOpSeparate(GL_FRONT, TranslateToGL(state.frontFaceStencilFailureOperation), TranslateToGL(state.frontFaceStencilDepthFailureOperation), TranslateToGL(state.frontFaceStencilPassOperation));
		glStencilFuncSeparate(GL_BACK, TranslateToGL(state.backFaceStencilCompareFunction), state.stencilRef, state.readMask);
		glStencilOpSeparate(GL_BACK, TranslateToGL(state.backFaceStencilFailureOperation), TranslateToGL(state.backFaceStencilDepthFailureOperation), TranslateToGL(state.backFaceStencilPassOperation));
	}

	if (cache.writeMask != state.writeMask)
		glStencilMask(state.writeMask);

	cache = state;
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(VertexBufferRef buffer)
{
	if( !buffer) return;
	assert(IsValid(buffer));

	if (m_cache.CurrentVBO == *buffer) return;
	m_cache.CurrentVBO = *buffer;
	glBindBuffer(GL_ARRAY_BUFFER, *buffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(IndexBufferRef buffer)
{
	if (!buffer) return;
	assert(IsValid(buffer));

	if (m_cache.CurrentIBO == *buffer) return;
	m_cache.CurrentIBO = *buffer;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(VertexArrayRef vao)
{
	if (m_cache.CurrentVAO != *vao)
	{
		m_cache.CurrentVAO = *vao;
		m_cache.CurrentVBO = 0;
		m_cache.CurrentIBO = 0;
		glBindVertexArray(*vao);
	}
	Bind(vao->vbo);
	Bind(vao->ibo);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(const VertexAttribute& attribute)
{
	const GLuint oglLocation = static_cast<GLuint>(attribute.location);
	glEnableVertexAttribArray(oglLocation);
	glVertexAttribPointer(
		oglLocation,
		attribute.size,
		GL_FLOAT,
		(GLboolean)(attribute.normalized ? GL_TRUE : GL_FALSE),
		attribute.stride,
		attribute.offset);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(Texture2DRef resource, unsigned slot)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentTexture2D[slot] == *resource ) return;
	m_cache.CurrentTexture2D[slot] = *resource;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, *resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(FramebufferRef resource)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentFramebuffer == *resource ) return;
	m_cache.CurrentFramebuffer = *resource;
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLVertexBuffer(unsigned id)
{
	if( m_cache.CurrentVBO == id ) return;
	m_cache.CurrentVBO = id;
	glBindBuffer(GL_ARRAY_BUFFER, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLIndexBuffer(unsigned id)
{
	if( m_cache.CurrentIBO == id ) return;
	m_cache.CurrentIBO = id;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLVertexArray(unsigned id)
{
	if( m_cache.CurrentVAO == id ) return;
	m_cache.CurrentVAO = id;
	m_cache.CurrentVBO = 0;
	m_cache.CurrentIBO = 0;
	glBindVertexArray(id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLTexture2D(unsigned id, unsigned slot)
{
	if( m_cache.CurrentTexture2D[slot] == id ) return;
	m_cache.CurrentTexture2D[slot] = id;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLFramebuffer(unsigned id)
{
	if( m_cache.CurrentFramebuffer == id ) return;
	m_cache.CurrentFramebuffer = id;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::Draw(VertexArrayRef vao, PrimitiveTopology primitive)
{
	assert(IsValid(vao));

	Bind(vao);
	if( vao->ibo)
	{
		glDrawElements(TranslateToGL(primitive), (GLsizei)vao->ibo->count, SizeIndexType(vao->ibo->sizeInBytes), nullptr);
	}
	else
	{
		glDrawArrays(TranslateToGL(primitive), 0, (GLsizei)vao->vbo->count);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::Draw(GeometryBufferRef geom, PrimitiveTopology primitive)
{
	if (!IsValid(geom)) return;
	Draw(geom->vao, primitive);
}
//-----------------------------------------------------------------------------
void RenderSystem::initializeExtensions(bool print)
{
	// reset extensions state
	OpenGLExtensions::version = OPENGL33;
	OpenGLExtensions::coreDebug = false;
	OpenGLExtensions::coreDirectStateAccess = false;

#if PLATFORM_DESKTOP
	if (!GLAD_GL_VERSION_3_3)
	{
		LogFatal("OpenGL 3.3+ not support");
		return;
	}
	if (GLAD_GL_VERSION_4_0) OpenGLExtensions::version = OPENGL40;
	if (GLAD_GL_VERSION_4_1) OpenGLExtensions::version = OPENGL41;
	if (GLAD_GL_VERSION_4_2) OpenGLExtensions::version = OPENGL42;
	if (GLAD_GL_VERSION_4_3) OpenGLExtensions::version = OPENGL43;
	if (GLAD_GL_VERSION_4_4) OpenGLExtensions::version = OPENGL44;
	if (GLAD_GL_VERSION_4_5) OpenGLExtensions::version = OPENGL45;
	if (GLAD_GL_VERSION_4_6) OpenGLExtensions::version = OPENGL46;
#else
	// TODO:
#endif // PLATFORM_DESKTOP

#if defined(_DEBUG)
	if (OpenGLExtensions::version >= OPENGL43 && glDebugMessageCallback)
	{
		glDebugMessageCallback(glDebugCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		OpenGLExtensions::coreDebug = true;
	}
#endif

	if (OpenGLExtensions::version >= OPENGL45)
		OpenGLExtensions::coreDirectStateAccess = true;

	if (print)
	{
		LogPrint("OpenGL: Extensions information:");
		LogPrint(std::string("    > OpenGL Debug: ") + (OpenGLExtensions::coreDebug ? "enable" : "disable"));
		LogPrint(std::string("    > OpenGL Direct State Access: ") + (OpenGLExtensions::coreDirectStateAccess ? "enable" : "disable"));
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::initializeCapabilities(bool print)
{
	if( print ) LogPrint("OpenGL: Capabilities information:");

	GLint openGLValue = 0;
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
	Capabilities::maximumTextureDimension = static_cast<uint32_t>(openGLValue);
	if( print ) LogPrint("    > Maximum Texture Dimension = " + std::to_string(Capabilities::maximumTextureDimension));

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &openGLValue);
	Capabilities::maximumTextureUnitCount = static_cast<uint32_t>(openGLValue);
	if( print ) LogPrint("    > Maximum Texture Unit Count = " + std::to_string(Capabilities::maximumTextureUnitCount));

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &openGLValue);
	Capabilities::maximumUniformBufferSize = static_cast<uint32_t>(openGLValue);
	if( print ) LogPrint("    > Maximum Uniform Buffer Size = " + std::to_string(Capabilities::maximumUniformBufferSize));
}
//-----------------------------------------------------------------------------
bool RenderSystem::checkCurrentFrameBuffer() const
{
	if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
	{
		LogError("FRAMEBUFFER:: Framebuffer is not complete!");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void RenderSystem::setClearMask(bool color, bool depth, bool stensil)
{
	m_cache.CurrentClearMask = color ? GL_COLOR_BUFFER_BIT : 0u;
	if (depth)   m_cache.CurrentClearMask |= GL_DEPTH_BUFFER_BIT;
	if (stensil) m_cache.CurrentClearMask |= GL_STENCIL_BUFFER_BIT;
}
//-----------------------------------------------------------------------------
RenderSystem& GetRenderSystem()
{
	return gRenderSystem;
}
//-----------------------------------------------------------------------------