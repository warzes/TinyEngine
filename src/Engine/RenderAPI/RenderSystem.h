#pragma once

#include "OpenGLCore.h"
#include "RenderResource.h"
#include "Capabilities.h"
#include "Core/IO/Image.h"

constexpr int MaxBindingTextures = 16;

struct RenderCreateInfo final
{
	glm::vec3 clearColor = glm::vec3{ 0.2f, 0.4f, 0.9f };
};

class RenderSystem final
{
	friend class EngineDevice;
public:
	RenderSystem() = default;

	//-------------------------------------------------------------------------
	// Core Systen
	//-------------------------------------------------------------------------
	bool Create(const RenderCreateInfo& createInfo);
	void Destroy();

	//-------------------------------------------------------------------------
	// Config Current Frame Set
	//-------------------------------------------------------------------------
	void SetClearColor(const glm::vec3& color);
	void ClearFrame();
	void ClearFrame(const glm::vec3& color);
	void SetViewport(int width, int height);
	void SetViewport(int x, int y, int width, int height);
	void MainScreen();

	//-------------------------------------------------------------------------
	// Create Render Resource
	//-------------------------------------------------------------------------
	ShaderProgramRef CreateShaderProgram(const ShaderBytecode& vertexShaderSource, const ShaderBytecode& fragmentShaderSource);
	VertexBufferRef CreateVertexBuffer(BufferUsage usage);
	VertexBufferRef CreateVertexBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data);
	IndexBufferRef CreateIndexBuffer(BufferUsage usage);
	IndexBufferRef CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexFormat indexFormat, const void* data);

	/*
	* разница между attribs и shaders. при передаче шейдера, при создании вао движок пытается создать описание формата вершины из кода шейдера. Такой вариант удобнее, но если в шейдере не использовался какой-либо атрибут вершины (например нигде не используется тангенс), то при компиляции glsl кода шейдера этот атрибут будет выкинут из-за чего чтение данных из вершины будет совершенно некоректным. Подумать как решить эту проблему
	*/
	VertexArrayRef CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, const std::vector<VertexAttribute>& attribs);
	VertexArrayRef CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, ShaderProgramRef shaders);

	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexFormat indexFormat, const void* indexData,
		ShaderProgramRef shader);
	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexFormat indexFormat, const void* indexData,
		const std::vector<VertexAttribute>& attribs);
	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, ShaderProgramRef shader);
	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs);

	GeometryBufferRef CreateGeometryBuffer(VertexBufferRef vertexBuffer, ShaderProgramRef shader);
	GeometryBufferRef CreateGeometryBuffer(VertexBufferRef vertexBuffer, const std::vector<VertexAttribute>& attribs);
	GeometryBufferRef CreateGeometryBuffer(VertexBufferRef vertexBuffer, IndexBufferRef indexBuffer, ShaderProgramRef shader);
	GeometryBufferRef CreateGeometryBuffer(VertexBufferRef vertexBuffer, IndexBufferRef indexBuffer, const std::vector<VertexAttribute>& attribs);

	Texture2DRef CreateTexture2D(const char* fileName, bool useCache = true, const Texture2DInfo& textureInfo = {});
	Texture2DRef CreateTexture2D(ImageRef image, const Texture2DInfo& textureInfo = {});
	Texture2DRef CreateTexture2D(ImageRef image, const char* nameInCache, const Texture2DInfo& textureInfo = {});
	Texture2DRef CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	RenderbufferRef CreateRenderbuffer(const glm::uvec2& size, ImageFormat format, int multisample = 1);

	// разница между текстурой и рендербуфером в фреймбуфере - текстуру можно сразу биндить

	FramebufferRef CreateFramebuffer(RenderbufferRef colorBuffer);
	FramebufferRef CreateFramebuffer(RenderbufferRef colorBuffer, RenderbufferRef depthStencilBuffer);
	FramebufferRef CreateFramebuffer(Texture2DRef colorTexture);
	FramebufferRef CreateFramebuffer(Texture2DRef colorTexture, RenderbufferRef depthStencilBuffer);
	FramebufferRef CreateFramebuffer(const std::vector<Texture2DRef>& colorTextures, RenderbufferRef depthStencilBuffer);
	FramebufferRef CreateFramebuffer(Texture2DRef colorTexture, Texture2DRef depthStencilTexture);

	FramebufferRef CreateFramebuffer(FramebufferAttachment attachment, Texture2DRef texture); // TODO: delete

	//-------------------------------------------------------------------------
	// Validation Resource
	//-------------------------------------------------------------------------
	inline bool IsValid(ShaderRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(ShaderProgramRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(const Uniform& uniform) const { return uniform.location >= 0; }
	inline bool IsValid(GPUBufferRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(VertexArrayRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(Texture2DRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(GeometryBufferRef resource) const { return resource && IsValid(resource->vao); }
	inline bool IsValid(FramebufferRef resource) const { return resource && resource->IsValid(); }
	bool IsReadyUniform(const Uniform& uniform) const;

	//-------------------------------------------------------------------------
	// Shader Operations
	//-------------------------------------------------------------------------
	std::vector<ShaderAttributeInfo> GetAttributesInfo(ShaderProgramRef program) const;
	Uniform GetUniform(ShaderProgramRef program, const char* uniformName) const;

	void SetUniform(const Uniform& uniform, bool value);
	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);
	void SetUniform3(const Uniform& uniform, unsigned number, float* values);
	void SetUniform4(const Uniform& uniform, unsigned number, float* values);

	void SetUniform(const Uniform& uniform, std::span<float> values);
	void SetUniform(const Uniform& uniform, std::span<glm::vec2> values);
	void SetUniform(const Uniform& uniform, std::span<glm::vec3> values);
	void SetUniform(const Uniform& uniform, std::span<glm::vec4> values);

	// не рекомендуется - только для быстрого теста
	void SetUniform(const std::string& uniformName, bool value);
	void SetUniform(const std::string& uniformName, int value);
	void SetUniform(const std::string& uniformName, float value);
	void SetUniform(const std::string& uniformName, const glm::vec2& value);
	void SetUniform(const std::string& uniformName, const glm::vec3& value);
	void SetUniform(const std::string& uniformName, const glm::vec4& value);
	void SetUniform(const std::string& uniformName, const glm::mat3& value);
	void SetUniform(const std::string& uniformName, const glm::mat4& value);

	//-------------------------------------------------------------------------
	// Buffer Operations
	//-------------------------------------------------------------------------
	// TODO: отрефакторить убрав копипаст выделив его в приватную функцию
	bool UpdateBuffer(VertexBufferRef buffer, unsigned offset, unsigned count, unsigned size, const void* data);
	bool UpdateBuffer(IndexBufferRef buffer, unsigned offset, unsigned count, IndexFormat indexFormat, const void* data);

	// TODO: нужно проверить как работает бинд перед мапингом с учетом текущего ВАО
#if !PLATFORM_EMSCRIPTEN
	void* MapBuffer(VertexBufferRef buffer, GLenum access = GL_WRITE_ONLY);
	void* MapBuffer(IndexBufferRef buffer, GLenum access = GL_WRITE_ONLY);
#endif
	void* MapBuffer(VertexBufferRef buffer, unsigned offset, unsigned size, GLbitfield access = GL_MAP_WRITE_BIT);
	void* MapBuffer(IndexBufferRef buffer, unsigned offset, unsigned size, GLbitfield access = GL_MAP_WRITE_BIT);
	bool UnmapBuffer(VertexBufferRef buffer);
	bool UnmapBuffer(IndexBufferRef buffer);

	//-------------------------------------------------------------------------
	// Set Current State 
	//-------------------------------------------------------------------------
	void ResetAllStates();
	void ResetState(ResourceType type);
	void Bind(DepthState state);
	void Bind(StencilState state);
	void Bind(RasterizerState state);
	void Bind(ShaderProgramRef resource);
	void Bind(unsigned rawShader);
	void Bind(VertexBufferRef buffer);
	void Bind(IndexBufferRef buffer);
	void Bind(VertexArrayRef vao);
	void Bind(const VertexAttribute& Attribute);
	void Bind(Texture2DRef resource, unsigned slot = 0);
	void Bind(FramebufferRef resource);

	//-------------------------------------------------------------------------
	// Raw GL State
	//-------------------------------------------------------------------------
	void BindGLVertexBuffer(unsigned id);
	void BindGLIndexBuffer(unsigned id);
	void BindGLVertexArray(unsigned id);
	void BindGLTexture2D(unsigned id, unsigned slot);
	void BindGLFramebuffer(unsigned id);

	//-------------------------------------------------------------------------
	// Draw
	//-------------------------------------------------------------------------
	void Draw(VertexArrayRef vao, PrimitiveTopology primitive = PrimitiveTopology::Triangles);
	void Draw(GeometryBufferRef geom, PrimitiveTopology primitive = PrimitiveTopology::Triangles);

	//-------------------------------------------------------------------------
	// Binding state
	//-------------------------------------------------------------------------
	unsigned GetCurrentIBO() const { return m_cache.CurrentIBO; }
private:
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

	void initializeExtensions(bool print);
	void initializeCapabilities(bool print);

	void validationShaderCode(std::string& vertexCode, std::string& fragmentCode);

	ShaderRef compileShader(ShaderPipelineStage type, const std::string& source);
	void attachmentFrameBufferColor(FramebufferRef fbo, RenderbufferRef colorBuffer);
	void attachmentFrameBufferColor(FramebufferRef fbo, Texture2DRef colorTexture);
	void attachmentFrameBufferColor(FramebufferRef fbo, const std::vector<Texture2DRef>& colorTextures);
	void attachmentFrameBufferDepthStencil(FramebufferRef fbo, RenderbufferRef depthStencilBuffer);
	void attachmentFrameBufferDepthStencil(FramebufferRef fbo, Texture2DRef depthStencilTexture);
	bool checkCurrentFrameBuffer() const;

	void setClearMask(bool color, bool depth, bool stensil);

	struct
	{
		unsigned CurrentShaderProgram = 0;
		unsigned CurrentVBO = 0;
		unsigned CurrentIBO = 0;
		unsigned CurrentVAO = 0;
		unsigned CurrentTexture2D[MaxBindingTextures] = { 0 };
		unsigned CurrentFramebuffer = 0;

		DepthState CurrentDepthState{};
		StencilState CurrentStencilState{};
		RasterizerState CurrentRasterizerState{};

		GLbitfield CurrentClearMask = 0;

		void Reset()
		{
			CurrentShaderProgram = CurrentVBO = CurrentIBO = CurrentVAO = CurrentFramebuffer = 0;
			for (size_t i = 0; i < MaxBindingTextures; i++)
				CurrentTexture2D[i] = 0;
			CurrentDepthState = {};
			CurrentStencilState = {};
			CurrentRasterizerState = {};
			CurrentClearMask = 0;
		}
	} m_cache;

	std::unordered_map<std::string, Texture2DRef> m_cacheFileTextures2D;
};

RenderSystem& GetRenderSystem();