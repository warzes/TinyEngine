#pragma once

extern void LogFatal(const std::string& msg) noexcept; // ==> Core/Logging/Log.h

//=============================================================================
// Core enum
//=============================================================================

enum class ResourceType : uint8_t
{
	Unknown,
	Shader,
	ShaderProgram,
	VertexBuffer,
	IndexBuffer,
	VertexArray,
	Texture2D,
	Framebuffer
};

enum class ImageFormat : uint8_t
{
	None = 0,
	R8,
	RG8,
	RGBA8,
	A8,
#if !PLATFORM_EMSCRIPTEN
	R16,
	RG16,
	RGBA16,
#endif
	R16F,
	RG16F,
	RGBA16F,
	R32F,
	RG32F,
	RGB32F,
	RGBA32F,
	R32U,
	RG32U,
	RGBA32U,
	D16,
#if !PLATFORM_EMSCRIPTEN
	D32,
#endif
	D24S8,

#if !PLATFORM_EMSCRIPTEN
	DXT1,
	DXT3,
	DXT5,
#endif
};

enum class ColorMask : uint8_t
{
	None = 0x00,
	Red = 0x01,
	Green = 0x02,
	Blue = 0x04,
	Alpha = 0x08,
	All = Red | Green | Blue | Alpha
};

//=============================================================================
// Depth Stencil enum
//=============================================================================

enum class ComparisonFunction : uint8_t
{
	Never,
	Equal,
	Less,
	LessEqual,
	Greater,
	GreaterEqual,
	NotEqual,
	Always,
};

enum class StencilOperation : uint8_t
{
	Keep,
	Zero,
	Replace,
	IncrementSaturation,
	DecrementSaturation,
	Invert,
	IncrementWrap,
	DecrementWrap
};

//=============================================================================
// Blend enum
//=============================================================================

enum class BlendOperation : uint8_t
{
	Add,
	Subrtact,
	ReverseSubtract,
	Min,
	Max
};

enum class BlendFactor : uint8_t
{
	Zero,
	One,
	SourceColor,
	InverseSourceColor,
	DestinationColor,
	InverseDestinationColor,
	SourceAlpha,
	InverseSourceAlpha,
	DestinationAlpha,
	InverseDestinationAlpha,
	ConstantColor,
	InverseConstantColor,
	ConstantAlpha,
	InverseConstantAlpha,
	SourceAlphaSaturation,
#if !PLATFORM_EMSCRIPTEN
	Source1Color,
	InverseSource1Color,
	Source1Alpha,
	InverseSource1Alpha
#endif
};

//=============================================================================
// Rasterizer enum
//=============================================================================

enum class RasterizerCullMode : uint8_t
{
	Back,
	Front,
	FrontAndBack
};

enum class FaceOrientation : uint8_t
{
	ClockwiseFace,
	CounterClockwiseFace,
};

enum class RasterizerFillMode : uint8_t
{
	Solid,
	Wireframe,
	Point
};

//=============================================================================
// Shader enum
//=============================================================================

enum class ShaderSourceType : uint8_t
{
	CodeMemory,   // Refers to <code>sourceSize+1</code> bytes, describing shader high-level code (including null terminator).
	CodeFile,     // Refers to <code>sourceSize+1</code> bytes, describing the filename of the shader high-level code (including null terminator).
	BinaryBuffer, // Refers to <code>sourceSize</code> bytes, describing shader binary code.
	BinaryFile,   // Refers to <code>sourceSize+1</code> bytes, describing the filename of the shader binary code (including null terminator).
};

enum class ShaderPipelineStage : uint8_t
{
	Vertex,        // Vertex shader type
	Fragment,      // Fragment shader type (also "Pixel Shader")
#if !PLATFORM_EMSCRIPTEN
	Geometry,      // Geometry shader type
	Compute,       // Compute shader type
	TessControl,   // Tessellation control shader type (also "Hull Shader")
	TessEvaluation // Tessellation evaluation shader type (also "Domain Shader")
#endif // PLATFORM_EMSCRIPTEN
};

//=============================================================================
// GPUBuffer enum
//=============================================================================

enum class BufferTarget : uint8_t
{
	ArrayBuffer,             // GL_ARRAY_BUFFER aka VertexBuffer
#if !PLATFORM_EMSCRIPTEN
	AtomicCounterBuffer,     // GL_ATOMIC_COUNTER_BUFFER
#endif
	CopyReadBuffer,          // GL_COPY_READ_BUFFER
	CopyWriteBuffer,         // GL_COPY_WRITE_BUFFER
#if !PLATFORM_EMSCRIPTEN
	DispatchIndirectBuffer,  // GL_DISPATCH_INDIRECT_BUFFER
	DrawIndirectBuffer,      // GL_DRAW_INDIRECT_BUFFER
#endif
	ElementArrayBuffer,      // GL_ELEMENT_ARRAY_BUFFER aka Index Buffer
	PixelPackBuffer,         // GL_PIXEL_PACK_BUFFER
	PixelUnpackBuffer,       // GL_PIXEL_UNPACK_BUFFER
	QueryBuffer,             // GL_QUERY_BUFFER
#if !PLATFORM_EMSCRIPTEN
	ShaderStorageBuffer,     // GL_SHADER_STORAGE_BUFFER
	TextureBuffer,           // GL_TEXTURE_BUFFER
#endif
	TransformFeedbackBuffer, // GL_TRANSFORM_FEEDBACK_BUFFER
	UniformBuffer            // GL_UNIFORM_BUFFER
};

enum class BufferUsage : uint8_t
{
	StaticDraw,
	DynamicDraw,
	StreamDraw,

	StaticRead,
	DynamicRead,
	StreamRead,

	StaticCopy,
	StreamCopy,
	DynamicCopy
};

#if !PLATFORM_EMSCRIPTEN
enum class BufferMapAccess : uint8_t
{
	Read,
	Write,
	ReadWrite
};
#endif

enum class BufferBinding : uint8_t
{
	ArrayBufferBinding,
#if !PLATFORM_EMSCRIPTEN
	AtomicCounterBufferBinding,
#endif
	CopyReadBuffer,
	CopyWriteBuffer,
#if !PLATFORM_EMSCRIPTEN
	DispatchIndirectBufferBinding,
	DrawIndirectBufferBinding,
#endif
	ElementArrayBufferBinding,
	PixelPackBufferBinding,
	PixelUnpackBufferBinding,
#if !PLATFORM_EMSCRIPTEN
	ShaderStorageBufferBinding,
	TextureBindingBuffer,
#endif
	TransformFeedbackBufferBinding,
	UniformBufferBinding
};

enum class BufferMapAccessFlags : uint8_t
{
	MapReadBit,
	MapWriteBit,
	MapInvalidateRangeBit,
	MapInvalidateBufferBit,
	MapFlushExplicitBit,
	MapUnsynchronizedBit,
#if !PLATFORM_EMSCRIPTEN
	MapPersistentBit,
	MapCoherentBit
#endif
};

//=============================================================================
// IndexBuffer enum
//=============================================================================

enum class IndexFormat : uint8_t
{
	UInt8,
	UInt16,
	UInt32
};

//=============================================================================
// VertexArray enum
//=============================================================================
enum class PrimitiveTopology : uint8_t
{
	Points,
	Lines,
	LineStrip,
	LineLoop,
	Triangles,
	TriangleStrip,
	TriangleFan,
#if !PLATFORM_EMSCRIPTEN
	LinesAdjacency,
	LineStripAdjacency,
	TrianglesAdjacency,
	TriangleStripAdjacency,
	Patches
#endif
};

//=============================================================================
// Texture enum
//=============================================================================

enum class TextureType : uint8_t // TODO: использовать
{
#if !PLATFORM_EMSCRIPTEN
	Texture1D,
#endif
	Texture2D,
	Texture3D,
#if !PLATFORM_EMSCRIPTEN
	TextureRectangle,
	Texture1DArray,
#endif
	Texture2DArray,
	TextureCubeMap,
};

enum class TextureMinFilter : uint8_t
{
	Nearest,
	Linear,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapNearest,
	LinearMipmapLinear,
};

enum class TextureMagFilter : uint8_t
{
	Nearest,
	Linear,
};

enum class TextureAddressMode : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
#if !PLATFORM_EMSCRIPTEN // not support in webGL
	ClampToBorder
#endif
};

enum class TexelsFormat : uint8_t
{
	None = 0,
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,

	R_F32,
	RG_F32,

	Depth_U16,
	DepthStencil_U16,
	Depth_U24,
	DepthStencil_U24,
};

enum class TextureCubeTarget : uint8_t
{
	TextureCubeMapPositiveX,
	TextureCubeMapNegativeX,
	TextureCubeMapPositiveY,
	TextureCubeMapNegativeY,
	TextureCubeMapPositiveZ,
	TextureCubeMapNegativeZ
};

//=============================================================================
// Framebuffer enum
//=============================================================================

enum class FramebufferAttachment : uint8_t
{
	ColorAttachment0,
	ColorAttachment1,
	ColorAttachment2,
	ColorAttachment3,
	ColorAttachment4,
	ColorAttachment5,
	ColorAttachment6,
	ColorAttachment7,
	ColorAttachment8,
	ColorAttachment9,
	ColorAttachment10,
	ColorAttachment11,
	ColorAttachment12,
	ColorAttachment13,
	ColorAttachment14,
	ColorAttachment15,
	DepthAttachment,
	StencilAttachment,
	DepthStencilAttachment
};

enum class FramebufferType : uint8_t
{
	ReadFramebuffer,
	DrawFramebuffer,
	Framebuffer
};

enum class FramebufferStatus : uint8_t
{
	FramebufferComplete,
	FramebufferUndefined,
	FramebufferIncompleteAttachment,
	FramebufferIncompleteMissingAttachment,
#if !PLATFORM_EMSCRIPTEN
	FramebufferIncompleteDrawBuffer,
	FramebufferIncompleteReadBuffer,
#endif
	FramebufferUnsupported,
	FramebufferIncompleteMultisample,
#if !PLATFORM_EMSCRIPTEN
	FramebufferIncompleteLayerTargets
#endif
};

enum class FramebufferBinding : uint8_t
{
	ReadFramebufferBinding,
	DrawFramebufferBinding,
	FramebufferBinding
};

//=============================================================================
// Pipeline State Core
//=============================================================================

struct DepthState final
{
	ComparisonFunction compareFunction = ComparisonFunction::Less;
	bool enable = true;
	bool depthWrite = true;
};

struct StencilState final
{
	bool enable = false;
	uint32_t readMask = 0xFFFFFFFFU;
	uint32_t writeMask = 0xFFFFFFFFU;

	ComparisonFunction frontFaceStencilCompareFunction = ComparisonFunction::Always;
	StencilOperation   frontFaceStencilPassOperation = StencilOperation::Keep;  // stencil and depth pass
	StencilOperation   frontFaceStencilFailureOperation = StencilOperation::Keep;  // stencil fail (depth irrelevant)
	StencilOperation   frontFaceStencilDepthFailureOperation = StencilOperation::Keep; // stencil pass, depth fail

	ComparisonFunction backFaceStencilCompareFunction = ComparisonFunction::Always;
	StencilOperation   backFaceStencilPassOperation = StencilOperation::Keep;
	StencilOperation   backFaceStencilFailureOperation = StencilOperation::Keep;
	StencilOperation   backFaceStencilDepthFailureOperation = StencilOperation::Keep;

	int stencilRef = 0;
};

struct DepthStencilState final
{
	DepthState depthState;
	StencilState stencilState;
};

struct BlendState final
{
	BlendFactor colorBlendSource = BlendFactor::One;
	BlendFactor colorBlendDest = BlendFactor::Zero;
	BlendOperation colorOperation = BlendOperation::Add;

	BlendFactor alphaBlendSource = BlendFactor::One;
	BlendFactor alphaBlendDest = BlendFactor::Zero;
	BlendOperation alphaOperation = BlendOperation::Add;

	ColorMask colorMask = ColorMask::All;
	bool enable = false;
};
























// Depth bias descriptor structure to control fragment depth values.
struct DepthBiasDescriptor
{
	// Specifies a scalar factor controlling the constant depth value added to each fragment. By default 0.0.
	float constantFactor = 0.0f;
	// Specifies a scalar factor applied to a fragment's slope in depth bias calculations. By default 0.0.
	float slopeFactor = 0.0f;
	// Specifies the maximum (or minimum) depth bias of a fragment. By default 0.0.	
	float clamp = 0.0f;
};

inline bool IsPolygonOffsetEnabled(const DepthBiasDescriptor& desc)
{
	// Ignore clamp factor for this check, since it's useless without the other two parameters
	return (desc.slopeFactor != 0.0f || desc.constantFactor != 0.0f);
}

struct RasterizerState final
{
	RasterizerFillMode polygonMode = RasterizerFillMode::Solid;
	RasterizerCullMode cullMode = RasterizerCullMode::FrontAndBack;
	// Specifies the parameters to bias fragment depth values.
	DepthBiasDescriptor depthBias;
	FaceOrientation face = FaceOrientation::CounterClockwiseFace;
	// If enabled, primitives are discarded after optional stream-outputs but before the rasterization stage.
	bool discardEnabled = false;
	// If enabled, there is effectively no near and far clipping plane.
	bool depthClampEnabled = false;
	// Specifies whether scissor test is enabled or disabled.
	bool scissorTestEnabled = false;
	// Specifies whether multi-sampling is enabled or disabled.
	bool multiSampleEnabled = false;
	// Specifies whether lines are rendered with or without anti-aliasing.
	bool antiAliasedLineEnabled = false;
	// Specifies the width of all generated line primitives. 
	float lineWidth = 1.0f;
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
	ShaderBytecode(const std::string& src) : m_sourceCode(src), m_shaderSource(ShaderSourceType::CodeMemory) {};
	ShaderBytecode(ShaderBytecode&&) = default;
	ShaderBytecode(const ShaderBytecode&) = default;
	ShaderBytecode(ShaderSourceType shaderSource, const std::string& text);

	ShaderBytecode& operator=(ShaderBytecode&&) = default;
	ShaderBytecode& operator=(const ShaderBytecode&) = default;
	ShaderBytecode& operator=(const std::string& src) { m_sourceCode = src; m_shaderSource = ShaderSourceType::CodeMemory; return *this; }

	bool LoadFromFile(const std::string& file);

	std::string& GetSource() { return m_sourceCode; }
	const std::string& GetSource() const { return m_sourceCode; }
	const std::string& GetPath() const { return m_path; }
	const std::string& GetFilename() const { return m_filename; }
	ShaderSourceType GetShaderSourceType() const { return m_shaderSource; }

	bool IsValid() const { return m_sourceCode.length() > 0; }

	template<typename T>
	void InsertMacroValue(const std::string& macroName, const T& value)
	{
		size_t macroPos = m_sourceCode.find("#define " + macroName);
#if defined(_DEBUG)
		if (macroPos == std::string::npos)
		{
			LogFatal("ShaderBytecode::InsertMacroValue is called for '" + m_filename + "', but the shader doesn't have any macro named " + macroName);
			return;
		}
#endif
		size_t macroEnd = m_sourceCode.find('\n', macroPos);

		std::stringstream sstream;
		sstream << m_sourceCode.substr(0, macroPos + strlen("#define ") + macroName.length());
		sstream << ' ' << value << m_sourceCode.substr(macroEnd);
		m_sourceCode = sstream.str();
	}

	static std::string GetHeaderVertexShader();
	static std::string GetHeaderFragmentShader();

private:
	std::string m_filename = "Unnamed shader";
	std::string m_path;
	std::string m_sourceCode;
	ShaderSourceType m_shaderSource = ShaderSourceType::CodeMemory;
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

	bool verticallyFlip = false; // TODO: пока работает только при загрузке stb image
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