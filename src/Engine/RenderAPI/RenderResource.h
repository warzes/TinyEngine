#pragma once

#include "OpenGLUtil.h"
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
	ShaderBytecode(const std::string& src) : m_src(src) {};
	ShaderBytecode(ShaderBytecode&&) = default;
	ShaderBytecode(const ShaderBytecode&) = default;

	ShaderBytecode& operator=(ShaderBytecode&&) = default;
	ShaderBytecode& operator=(const ShaderBytecode&) = default;
	ShaderBytecode& operator=(const std::string& src) { m_src = src; return *this; }

	bool LoadFromFile(const std::string& file);

	std::string& GetSource() { return m_src; }
	const std::string& GetSource() const { return m_src; }
	const std::string& GetPath() const { return m_path; }
	const std::string& GetFilename() const { return m_filename; }

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

#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(Uniform) == 8, "Uniform size changed!!!");
#endif

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
		, m_ownership{ other.m_ownership }
	{
		other.m_handle = 0;
		other.m_ownership = false;
	}
	glObject& operator=(glObject&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_ownership, other.m_ownership);
		return *this;
	}

	operator GLuint() const { return m_handle; }
	GLuint Id() const { return m_handle; }
	bool HasOwnership() const { return m_ownership; }
	bool IsValid() const { return m_handle > 0; }

protected:
	glObject() = default;
	GLuint m_handle = 0;
	// Specifies if this object has the ownership of the entity behind the handle
	bool m_ownership = false;
};

class Shader final : public glObject
{
public:
	explicit Shader(GLuint handle) { m_handle = handle; m_ownership = false; }
	explicit Shader(ShaderPipelineStage stage);
	~Shader() { if( m_ownership ) glDeleteShader(m_handle); }

	Shader(Shader&&) noexcept = default;
	Shader& operator=(Shader&&) noexcept = default;
};
using ShaderRef = std::shared_ptr<Shader>;

class ShaderProgram final : public glObject
{
public:
	ShaderProgram() { m_handle = glCreateProgram(); m_ownership = true; }
	explicit ShaderProgram(GLuint handle) { m_handle = handle; m_ownership = false; }
	ShaderProgram(ShaderProgram&&) noexcept = default;
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram() { if( m_ownership ) glDeleteProgram(m_handle); }
	ShaderProgram& operator=(ShaderProgram&&) noexcept = default;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	bool operator==(const ShaderProgram& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership; }
};
using ShaderProgramRef = std::shared_ptr<ShaderProgram>;

#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(ShaderProgram) == 8, "ShaderProgram size changed!!!");
#endif

class VertexArray;

class GPUBuffer final : public glObject
{
public:
	GPUBuffer() = delete;
	GPUBuffer(GLuint handle, BufferType Type, BufferUsage Usage, unsigned Count, unsigned Size) : type(Type), usage(Usage), count(Count), size(Size) { m_handle = handle; m_ownership = false; }
	GPUBuffer(BufferType Type, BufferUsage Usage, unsigned Count, unsigned Size) : type(Type), usage(Usage), count(Count), size(Size) { glGenBuffers(1, &m_handle); m_ownership = true; }
	GPUBuffer(GPUBuffer&&) noexcept = default;
	GPUBuffer(const GPUBuffer&) = delete;
	~GPUBuffer() { if( m_ownership ) glDeleteBuffers(1, &m_handle); }
	GPUBuffer& operator=(GPUBuffer&&) noexcept = default;
	GPUBuffer& operator=(const GPUBuffer&) = delete;

	bool operator==(const GPUBuffer& ref) noexcept { return m_handle == ref.m_handle &&  m_ownership == ref.m_ownership &&  usage == ref.usage && count == ref.count && size == ref.size && type == ref.type; }

	BufferType type = BufferType::ArrayBuffer;
	BufferUsage usage = BufferUsage::StaticDraw;
	unsigned count = 0;
	unsigned size = 0; 
	std::shared_ptr<VertexArray> parentArray = nullptr;
};
using GPUBufferRef = std::shared_ptr<GPUBuffer>;
#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(GPUBuffer) == 40, "GPUBuffer size changed!!!");
#endif

// TODO: возможно снова разделить на вершинный буфер и т.д. так как VAO нужно только вершинному и не нужно например юниформ буферу

class VertexArray final : public glObject
{
public:
	VertexArray() = delete;
	VertexArray(GLuint handle, GPUBufferRef Vbo, GPUBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { m_handle = handle; m_ownership = false; }
	VertexArray(GPUBufferRef Vbo, GPUBufferRef Ibo, unsigned AttribsCount) : vbo(Vbo), ibo(Ibo), attribsCount(AttribsCount) { glGenVertexArrays(1, &m_handle); m_ownership = true; }
	VertexArray(VertexArray&&) = default;
	VertexArray(const VertexArray&) = delete;
	~VertexArray() { if( m_ownership ) glDeleteVertexArrays(1, &m_handle); }
	VertexArray& operator=(VertexArray&&) = default;
	VertexArray& operator=(const VertexArray&) = delete;

	bool operator==(const VertexArray& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && vbo == ref.vbo && ibo == ref.ibo && attribsCount == ref.attribsCount; }

	GPUBufferRef vbo = nullptr;
	GPUBufferRef ibo = nullptr;
	unsigned attribsCount = 0;
};
using VertexArrayRef = std::shared_ptr<VertexArray>;
#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(VertexArray) == 48, "VertexArray size changed!!!");
#endif

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
	Texture() { glGenTextures(1, &m_handle); m_ownership = true; }
	explicit Texture(GLuint handle) { m_handle = handle; m_ownership = false; }
	Texture(Texture&&) noexcept = default;
	Texture(const Texture&) = delete;
	~Texture() { if( m_ownership ) glDeleteTextures(1, &m_handle); }
	Texture& operator=(Texture&&) noexcept = default;
	Texture& operator=(const Texture&) = delete;
};

class Texture2D final : public Texture
{
public:
	Texture2D() = delete;
	Texture2D(GLuint handle, unsigned Width, unsigned Height, TexelsFormat Format) : Texture(handle), width(Width), height(Height), format(Format) {}
	Texture2D(unsigned Width, unsigned Height, TexelsFormat Format) : Texture(), width(Width), height(Height), format(Format) { }
	Texture2D(Texture2D&&) = default;
	Texture2D(const Texture2D&) = delete;
	~Texture2D() { if( m_ownership ) glDeleteTextures(1, &m_handle); }
	Texture2D& operator=(Texture2D&&) = default;
	Texture2D& operator=(const Texture2D&) = delete;

	bool operator==(const Texture2D& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && width == ref.width && height == ref.height && format == ref.format; }

	unsigned width = 0;
	unsigned height = 0;
	TexelsFormat format = TexelsFormat::RGBA_U8; 
};

using Texture2DRef = std::shared_ptr<Texture2D>;

#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(Texture2D) == 20, "Texture2D size changed!!!");
#endif

class Renderbuffer final : public glObject
{
public:
	Renderbuffer() { glGenRenderbuffers(1, &m_handle); m_ownership = true; }
	explicit Renderbuffer(GLuint handle) { m_handle = handle; m_ownership = false; }
	Renderbuffer(Renderbuffer&&) noexcept = default;
	Renderbuffer(const Renderbuffer&) = delete;
	~Renderbuffer() { if( m_ownership ) glDeleteRenderbuffers(1, &m_handle); }
	Renderbuffer& operator=(Renderbuffer&&) noexcept = default;
	Renderbuffer& operator=(const Renderbuffer&) = delete;

	bool operator==(const Renderbuffer& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && format == ref.format && size == ref.size && multisample == ref.multisample; }

	unsigned Width() const { return size.x; }
	unsigned Height() const { return size.y; }

	ImageFormat format;
	glm::uvec2 size = glm::uvec2{ 0 };
	int multisample = 1;
};
using RenderbufferRef = std::shared_ptr<Renderbuffer>;

#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(Renderbuffer) == 24, "Renderbuffer size changed!!!");
#endif

class Framebuffer final : public glObject
{
public:
	Framebuffer() { glGenFramebuffers(1, &m_handle); m_ownership = true; }
	explicit Framebuffer(GLuint handle) { m_handle = handle; m_ownership = false; }
	Framebuffer(Framebuffer&&) = default;
	Framebuffer(const Framebuffer&) = delete;
	~Framebuffer() { if( m_ownership ) glDeleteFramebuffers(1, &m_handle); }
	Framebuffer& operator=(Framebuffer&&) = default;
	Framebuffer& operator=(const Framebuffer&) = delete;

	bool operator==(const Framebuffer& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership && size == ref.size && colorTextures == ref.colorTextures && colorBuffer == ref.colorBuffer && depthStencilTexture == ref.depthStencilTexture && depthStencilBuffer ==ref.depthStencilBuffer; }

	glm::uvec2 size = glm::uvec2(0);
	std::vector<Texture2DRef> colorTextures = { nullptr };
	RenderbufferRef colorBuffer = nullptr;
	Texture2DRef depthStencilTexture = nullptr;
	RenderbufferRef depthStencilBuffer = nullptr;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;

#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(Framebuffer) == 96, "Framebuffer size changed!!!");
#endif

#if USE_OPENGL_VERSION == OPENGL40
class TransformFeedback final : public glObject
{
public:
	TransformFeedback() { glGenTransformFeedbacks(1, &m_handle); m_ownership = true; }
	explicit TransformFeedback(GLuint handle) { m_handle = handle; m_ownership = false; }
	TransformFeedback(TransformFeedback&&) noexcept = default;
	TransformFeedback(const TransformFeedback&) = delete;
	~TransformFeedback() { if( m_ownership ) glDeleteTransformFeedbacks(1, &m_handle); }
	TransformFeedback& operator=(TransformFeedback&&) noexcept = default;
	TransformFeedback& operator=(const TransformFeedback&) = delete;

	bool operator==(const TransformFeedback& ref) noexcept { return m_handle == ref.m_handle && m_ownership == ref.m_ownership; }
};
using TransformFeedbackRef = std::shared_ptr<TransformFeedback>;

#if !PLATFORM_EMSCRIPTEN
static_assert(sizeof(TransformFeedback) == 8, "TransformFeedback size changed!!!");
#endif

#endif // OPENGL40