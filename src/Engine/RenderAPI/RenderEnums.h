#pragma once

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

enum class StencilOp : uint8_t
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

enum class BlendOp : uint8_t
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
	CodeString,   // Refers to <code>sourceSize+1</code> bytes, describing shader high-level code (including null terminator).
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

// TODO: использовать
enum class BufferType : uint8_t
{
	ArrayBuffer, // aka VertexBuffer
#if !PLATFORM_EMSCRIPTEN
	AtomicCounterBuffer,
#endif
	CopyReadBuffer,
	CopyWriteBuffer,
#if !PLATFORM_EMSCRIPTEN
	DispatchIndirectBuffer,
	DrawIndirectBuffer,
#endif
	ElementArrayBuffer, // aka Index Buffer
	PixelPackBuffer,
	PixelUnpackBuffer,
#if !PLATFORM_EMSCRIPTEN
	ShaderStorageBuffer,
	TextureBuffer,
#endif
	TransformFeedbackBuffer,
	UniformBuffer
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