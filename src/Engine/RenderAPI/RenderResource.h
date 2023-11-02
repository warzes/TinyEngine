#pragma once

#include "OpenGLCore.h"
#include "RenderCore.h"
#include "Core/Logging/Log.h"

// TODO: в деструкторах рендерресурсов при удалении ресурса снимать бинд если он забинден

//=============================================================================
// Render Resource OpenGL Object
//=============================================================================

class glObject
{
public:
	glObject(const glObject&) = delete;
	glObject& operator=(const glObject&) = delete;

	glObject(glObject&& other) noexcept : m_handle{ other.m_handle }
	{
		other.m_handle = 0;
	}
	glObject& operator=(glObject&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		return *this;
	}

	operator GLuint() const { return m_handle; }
	GLuint Id() const { return m_handle; }
	bool IsValid() const { return m_handle > 0; }

protected:
	glObject() = default;
	GLuint m_handle = 0;
};

class Shader final : public glObject
{
public:
	explicit Shader(ShaderPipelineStage stage);
	~Shader() { glDeleteShader(m_handle); }

	Shader(Shader&&) noexcept = default;
	Shader& operator=(Shader&&) noexcept = default;

	ShaderPipelineStage shaderStage = ShaderPipelineStage::Vertex;
};
using ShaderRef = std::shared_ptr<Shader>;

class ShaderProgram final : public glObject
{
public:
	ShaderProgram() { m_handle = glCreateProgram(); }
	ShaderProgram(ShaderProgram&&) noexcept = default;
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram() { glDeleteProgram(m_handle); }
	ShaderProgram& operator=(ShaderProgram&&) noexcept = default;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	bool operator==(const ShaderProgram& ref) noexcept { return m_handle == ref.m_handle; }
};
using ShaderProgramRef = std::shared_ptr<ShaderProgram>;

class VertexArray;

class GPUBuffer : public glObject
{
public:
	GPUBuffer() = delete;
	GPUBuffer(BufferTarget Type, BufferUsage Usage, unsigned Count = 0, unsigned Size = 0);
	GPUBuffer(GPUBuffer&&) noexcept = default;
	GPUBuffer(const GPUBuffer&) = delete;
	~GPUBuffer() { glDeleteBuffers(1, &m_handle); }
	GPUBuffer& operator=(GPUBuffer&&) noexcept = default;
	GPUBuffer& operator=(const GPUBuffer&) = delete;

	bool operator==(const GPUBuffer& ref) noexcept { return m_handle == ref.m_handle && type == ref.type && usage == ref.usage && count == ref.count && sizeInBytes == ref.sizeInBytes; }

	BufferTarget type = BufferTarget::ArrayBuffer;
	BufferUsage usage = BufferUsage::StaticDraw;
	unsigned count = 0;
	unsigned sizeInBytes = 0; 
	std::shared_ptr<VertexArray> parentArray = nullptr;
};
using GPUBufferRef = std::shared_ptr<GPUBuffer>;

class VertexBuffer final : public GPUBuffer
{
public:
	VertexBuffer() = delete;
	VertexBuffer(BufferUsage Usage, unsigned Count = 0, unsigned Size = 0) : GPUBuffer(BufferTarget::ArrayBuffer, Usage, Count, Size) {}
	VertexBuffer(VertexBuffer&&) noexcept = default;
	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer() { glDeleteBuffers(1, &m_handle); }
	VertexBuffer& operator=(VertexBuffer&&) noexcept = default;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	bool operator==(const VertexBuffer& ref) noexcept { return m_handle == ref.m_handle && type == ref.type && usage == ref.usage && count == ref.count && sizeInBytes == ref.sizeInBytes; }
};
using VertexBufferRef = std::shared_ptr<VertexBuffer>;

// GPU buffer for index data.
class IndexBuffer final : public GPUBuffer
{
public:
	IndexBuffer() = delete;
	IndexBuffer(BufferUsage Usage, unsigned Count = 0, unsigned Size = 0) : GPUBuffer(BufferTarget::ElementArrayBuffer, Usage, Count, Size) {}
	IndexBuffer(IndexBuffer&&) noexcept = default;
	IndexBuffer(const IndexBuffer&) = delete;
	~IndexBuffer();
	IndexBuffer& operator=(IndexBuffer&&) noexcept = default;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	bool operator==(const IndexBuffer& ref) noexcept { return m_handle == ref.m_handle && usage == ref.usage && count == ref.count && sizeInBytes == ref.sizeInBytes && type == ref.type; }
};
using IndexBufferRef = std::shared_ptr<IndexBuffer>;

// TODO: buffer storage (OpenGl 4.4+) - ref http://steps3d.narod.ru/tutorials/buffer-storage-tutorial.html

class VertexArray final : public glObject
{
public:
	VertexArray() = delete;
	VertexArray(VertexBufferRef Vbo, IndexBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { glGenVertexArrays(1, &m_handle); }
	VertexArray(VertexArray&&) = default;
	VertexArray(const VertexArray&) = delete;
	~VertexArray() { glDeleteVertexArrays(1, &m_handle); }
	VertexArray& operator=(VertexArray&&) = default;
	VertexArray& operator=(const VertexArray&) = delete;

	bool operator==(const VertexArray& ref) noexcept { return m_handle == ref.m_handle && vbo == ref.vbo && ibo == ref.ibo && attribsCount == ref.attribsCount; }

	VertexBufferRef vbo = nullptr;
	IndexBufferRef ibo = nullptr;
	unsigned attribsCount = 0;
};
using VertexArrayRef = std::shared_ptr<VertexArray>;

class GeometryBuffer final
{
public:
	GeometryBuffer() = default;
	GeometryBuffer(GeometryBuffer&&) = default;
	GeometryBuffer(const GeometryBuffer&) = delete;
	GeometryBuffer& operator=(GeometryBuffer&&) = default;
	GeometryBuffer& operator=(const GeometryBuffer&) = delete;

	VertexBufferRef GetVBO() { return vao->vbo; }
	IndexBufferRef GetIBO() { return vao->ibo; }

	VertexArrayRef vao = nullptr;
};
using GeometryBufferRef = std::shared_ptr<GeometryBuffer>;

class Texture : public glObject
{
public:
	Texture() { glGenTextures(1, &m_handle); }
	Texture(Texture&&) noexcept = default;
	Texture(const Texture&) = delete;
	~Texture() { glDeleteTextures(1, &m_handle); }
	Texture& operator=(Texture&&) noexcept = default;
	Texture& operator=(const Texture&) = delete;
};

class Texture2D final : public Texture
{
public:
	Texture2D() = delete;
	Texture2D(unsigned Width, unsigned Height, TexelsFormat Format, bool HasTransparency = false) : Texture(), width(Width), height(Height), format(Format), hasTransparency(HasTransparency) { }
	Texture2D(Texture2D&&) = default;
	Texture2D(const Texture2D&) = delete;
	~Texture2D() { glDeleteTextures(1, &m_handle); }
	Texture2D& operator=(Texture2D&&) = default;
	Texture2D& operator=(const Texture2D&) = delete;

	bool operator==(const Texture2D& ref) noexcept { return m_handle == ref.m_handle && width == ref.width && height == ref.height && format == ref.format && hasTransparency == ref.hasTransparency; }

	unsigned width = 0;
	unsigned height = 0;
	TexelsFormat format = TexelsFormat::RGBA_U8;
	bool hasTransparency = false;
};

using Texture2DRef = std::shared_ptr<Texture2D>;

// GPU renderbuffer object for rendering and blitting, that cannot be sampled as a texture.
class Renderbuffer final : public glObject
{
public:
	Renderbuffer() { glGenRenderbuffers(1, &m_handle); }
	Renderbuffer(Renderbuffer&&) noexcept = default;
	Renderbuffer(const Renderbuffer&) = delete;
	~Renderbuffer() { glDeleteRenderbuffers(1, &m_handle); }
	Renderbuffer& operator=(Renderbuffer&&) noexcept = default;
	Renderbuffer& operator=(const Renderbuffer&) = delete;

	bool operator==(const Renderbuffer& ref) noexcept { return m_handle == ref.m_handle && format == ref.format && size == ref.size && multisampling == ref.multisampling; }

	unsigned Width() const { return size.x; }
	unsigned Height() const { return size.y; }

	ImageFormat format = ImageFormat::None;
	glm::uvec2 size = glm::uvec2{ 0 };
	int multisampling = 1;
};
using RenderbufferRef = std::shared_ptr<Renderbuffer>;

// GPU framebuffer object for rendering. Combines color and depth-stencil textures or buffers.
class Framebuffer final : public glObject
{
public:
	Framebuffer() { glGenFramebuffers(1, &m_handle); }
	Framebuffer(Framebuffer&&) = default;
	Framebuffer(const Framebuffer&) = delete;
	~Framebuffer() { glDeleteFramebuffers(1, &m_handle); }
	Framebuffer& operator=(Framebuffer&&) = default;
	Framebuffer& operator=(const Framebuffer&) = delete;

	bool operator==(const Framebuffer& ref) noexcept { return m_handle == ref.m_handle && size == ref.size && colorTextures == ref.colorTextures && colorBuffer == ref.colorBuffer && depthStencilTexture == ref.depthStencilTexture && depthStencilBuffer ==ref.depthStencilBuffer; }

	glm::uvec2 size = glm::uvec2(0);
	std::vector<Texture2DRef> colorTextures = { nullptr };
	RenderbufferRef colorBuffer = nullptr;
	Texture2DRef depthStencilTexture = nullptr;
	RenderbufferRef depthStencilBuffer = nullptr;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

#if USE_OPENGL_VERSION >= OPENGL40
class TransformFeedback final : public glObject
{
public:
	TransformFeedback() { glGenTransformFeedbacks(1, &m_handle); }
	TransformFeedback(TransformFeedback&&) noexcept = default;
	TransformFeedback(const TransformFeedback&) = delete;
	~TransformFeedback() { glDeleteTransformFeedbacks(1, &m_handle); }
	TransformFeedback& operator=(TransformFeedback&&) noexcept = default;
	TransformFeedback& operator=(const TransformFeedback&) = delete;

	bool operator==(const TransformFeedback& ref) noexcept { return m_handle == ref.m_handle; }
};
using TransformFeedbackRef = std::shared_ptr<TransformFeedback>;

#endif // OPENGL40