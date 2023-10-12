#pragma once

#include "OpenGLCore.h"
#include "RenderEnums.h"
#include "Core/Logging/Log.h"

//=============================================================================
// Pipeline State Core
//=============================================================================

struct DepthState final
{
	ComparisonFunction depthFunc = ComparisonFunction::Less;
	bool depthWrite = true;
	bool enable = true;
};

struct StencilState final
{
	bool enable = false;
	uint8_t readMask = 0xFF;
	uint8_t writeMask = 0xFF;
	
	ComparisonFunction stencilFuncFront = ComparisonFunction::Always;
	StencilOp stencilPassOpFront = StencilOp::Keep;  // stencil and depth pass
	StencilOp stencilFailOpFront = StencilOp::Keep;  // stencil fail (depth irrelevant)
	StencilOp stencilZFailOpFront = StencilOp::Keep; // stencil pass, depth fail
	
	ComparisonFunction stencilFuncBack = ComparisonFunction::Always;
	StencilOp stencilPassOpBack = StencilOp::Keep;
	StencilOp stencilFailOpBack = StencilOp::Keep;
	StencilOp stencilZFailOpBack = StencilOp::Keep;

	int stencilRef = 0;
};

struct RasterizerState final
{

};

struct BlendState final
{

};

//=============================================================================
// Shader Core
//=============================================================================

// A class that can load shader sources in from files, and do some preprocessing on them.
class ShaderBytecode final
{
public:
	ShaderBytecode() = default;
	// Loads in the shader from a memory data.
	ShaderBytecode(const std::string& src) : m_src(src), m_shaderSource(ShaderSourceType::CodeString) {};
	ShaderBytecode(ShaderBytecode&&) = default;
	ShaderBytecode(const ShaderBytecode&) = default;
	ShaderBytecode(ShaderSourceType shaderSource, const std::string& text);

	ShaderBytecode& operator=(ShaderBytecode&&) = default;
	ShaderBytecode& operator=(const ShaderBytecode&) = default;
	ShaderBytecode& operator=(const std::string& src) { m_src = src; m_shaderSource = ShaderSourceType::CodeString; return *this; }

	bool LoadFromFile(const std::string& file);

	std::string& GetSource() { return m_src; }
	const std::string& GetSource() const { return m_src; }
	const std::string& GetPath() const { return m_path; }
	const std::string& GetFilename() const { return m_filename; }
	ShaderSourceType GetShaderSourceType() const { return m_shaderSource; }

	bool IsValid() const { return m_src.length() > 0; }

	template<typename T>
	void InsertMacroValue(const std::string& macroName, const T& value)
	{
		size_t macroPos = m_src.find("#define " + macroName);
#if defined(_DEBUG)
		if( macroPos == std::string::npos )
		{
			LogFatal("ShaderBytecode::insert_macro_value is called for '" + m_filename + "', but the shader doesn't have any macro named " + macroName);
			return;
		}
#endif
		size_t macroEnd = m_src.find('\n', macroPos);

		std::stringstream sstream;
		sstream << m_src.substr(0, macroPos + strlen("#define ") + macroName.length());
		sstream << ' ' << value << m_src.substr(macroEnd);
		m_src = sstream.str();
	}

	static std::string GetHeaderVertexShader();
	static std::string GetHeaderFragmentShader();

private:
	std::string m_filename = "Unnamed shader";
	std::string m_path;
	std::string m_src;
	ShaderSourceType m_shaderSource = ShaderSourceType::CodeString;
};

struct ShaderAttributeInfo final
{
	unsigned typeId;
	unsigned type;
	int numType;
	std::string name;
	int location;
};

struct Uniform final
{ 
	int location = -1; 
	unsigned programId = 0; 
};

//=============================================================================
// Buffer Core
//=============================================================================

struct VertexAttribute final
{
	unsigned location/* = -1*/;  // если -1, то берется индекс массива атрибутов
	int size;
	//unsigned type;
	bool normalized;
	int stride;         // sizeof Vertex
	const void* offset; // (void*)offsetof(Vertex, TexCoord)}
};

//=============================================================================
// Texture Core
//=============================================================================

struct Texture2DInfo final
{
	TextureMinFilter minFilter = TextureMinFilter::NearestMipmapNearest;
	TextureMagFilter magFilter = TextureMagFilter::Nearest;
	TextureAddressMode wrapS = TextureAddressMode::Repeat;
	TextureAddressMode wrapT = TextureAddressMode::Repeat;

	bool mipmap = true;
};

struct Texture2DCreateInfo final
{
	TexelsFormat format = TexelsFormat::RGBA_U8;
	uint16_t width = 1;
	uint16_t height = 1;
	uint8_t* pixelData = nullptr;
	unsigned mipMapCount = 1; // TODO: only compressed
	bool hasTransparency = false;
};

//=============================================================================
// Render Resource OpenGL Object
//=============================================================================

class glObject
{
public:
	glObject(const glObject&) = delete;
	glObject& operator=(const glObject&) = delete;

	glObject(glObject&& other) noexcept
		: m_handle{ other.m_handle }
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
	GPUBuffer(BufferTarget Type, BufferUsage Usage, unsigned Count, unsigned Size);
	GPUBuffer(GPUBuffer&&) noexcept = default;
	GPUBuffer(const GPUBuffer&) = delete;
	~GPUBuffer() { glDeleteBuffers(1, &m_handle); }
	GPUBuffer& operator=(GPUBuffer&&) noexcept = default;
	GPUBuffer& operator=(const GPUBuffer&) = delete;

	bool operator==(const GPUBuffer& ref) noexcept { return m_handle == ref.m_handle && usage == ref.usage && count == ref.count && size == ref.size && type == ref.type; }

	BufferTarget type = BufferTarget::ArrayBuffer;
	BufferUsage usage = BufferUsage::StaticDraw;
	unsigned count = 0;
	unsigned size = 0; 
	std::shared_ptr<VertexArray> parentArray = nullptr;
};
using GPUBufferRef = std::shared_ptr<GPUBuffer>;

class VertexBuffer final : public GPUBuffer
{
public:
	VertexBuffer() = delete;
	VertexBuffer(BufferUsage Usage, unsigned Count, unsigned Size) : GPUBuffer(BufferTarget::ArrayBuffer, Usage, Count, Size) {}
	VertexBuffer(VertexBuffer&&) noexcept = default;
	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer() { glDeleteBuffers(1, &m_handle); }
	VertexBuffer& operator=(VertexBuffer&&) noexcept = default;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	bool operator==(const VertexBuffer& ref) noexcept { return m_handle == ref.m_handle && usage == ref.usage && count == ref.count && size == ref.size && type == ref.type; }
};
using VertexBufferRef = std::shared_ptr<VertexBuffer>;

class IndexBuffer final : public GPUBuffer
{
public:
	IndexBuffer() = delete;
	IndexBuffer(BufferUsage Usage, unsigned Count, unsigned Size) : GPUBuffer(BufferTarget::ElementArrayBuffer, Usage, Count, Size) {}
	IndexBuffer(IndexBuffer&&) noexcept = default;
	IndexBuffer(const IndexBuffer&) = delete;
	~IndexBuffer() { glDeleteBuffers(1, &m_handle); }
	IndexBuffer& operator=(IndexBuffer&&) noexcept = default;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	bool operator==(const IndexBuffer& ref) noexcept { return m_handle == ref.m_handle && usage == ref.usage && count == ref.count && size == ref.size && type == ref.type; }
};
using IndexBufferRef = std::shared_ptr<IndexBuffer>;

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

	GPUBufferRef GetVBO() { return vao->vbo; }
	GPUBufferRef GetIBO() { return vao->ibo; }

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

class Renderbuffer final : public glObject
{
public:
	Renderbuffer() { glGenRenderbuffers(1, &m_handle); }
	Renderbuffer(Renderbuffer&&) noexcept = default;
	Renderbuffer(const Renderbuffer&) = delete;
	~Renderbuffer() { glDeleteRenderbuffers(1, &m_handle); }
	Renderbuffer& operator=(Renderbuffer&&) noexcept = default;
	Renderbuffer& operator=(const Renderbuffer&) = delete;

	bool operator==(const Renderbuffer& ref) noexcept { return m_handle == ref.m_handle && format == ref.format && size == ref.size && multisample == ref.multisample; }

	unsigned Width() const { return size.x; }
	unsigned Height() const { return size.y; }

	ImageFormat format;
	glm::uvec2 size = glm::uvec2{ 0 };
	int multisample = 1;
};
using RenderbufferRef = std::shared_ptr<Renderbuffer>;

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