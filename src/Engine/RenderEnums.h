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
	R16,
	RG16,
	RGBA16,
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
	D32,
	D24S8,

	DXT1,
	DXT3,
	DXT5,
};

//=============================================================================
// Depth Stencil enum
//=============================================================================

enum class ComparisonFunc : uint8_t
{
	Never,
	Equal,
	Less,
	LEqual,
	Greater,
	GEqual,
	NotEqual,
	Always,
};

enum class StencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,
	IncrSat,
	DecrSat,
	Invert,
	IncrWrap,
	DecrWrap
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
	SrcColor,
	OneMinusSrcColor,
	DstColor,
	OneMinusDstColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	ConstantColor,
	OneMinusConstantColor,
	ConstantAlpha,
	OneMinusConstantAlpha,
	SrcAlphaSaturate,
	Src1Color,
	OneMinusSrc1Color,
	Src1Alpha,
	OneMinusSrc1Alpha
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
	Cw,
	Ccw,
};

enum class RasterizerFillMode : uint8_t // TODO: нет функции TranslateToGL
{
	Solid,
	Wireframe,
	Point
};

//=============================================================================
// Shader enum
//=============================================================================

enum class ShaderType : uint8_t
{
	Vertex,
	Fragment,
	Geometry,
	Compute,
	TessControl,
	Evaluation
};

//=============================================================================
// GPUBuffer enum
//=============================================================================

// TODO: использовать
enum class BufferType : uint8_t
{
	ArrayBuffer, // aka VertexBuffer
	AtomicCounterBuffer,
	CopyReadBuffer,
	CopyWriteBuffer,
	DispatchIndirectBuffer,
	DrawIndirectBuffer,
	ElementArrayBuffer, // aka Index Buffer
	PixelPackBuffer,
	PixelUnpackBuffer,
	ShaderStorageBuffer,
	TextureBuffer,
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

enum class BufferMapAccess : uint8_t
{
	Read,
	Write,
	ReadWrite
};

enum class BufferBinding : uint8_t
{
	ArrayBufferBinding,
	AtomicCounterBufferBinding,
	CopyReadBuffer,
	CopyWriteBuffer,
	DispatchIndirectBufferBinding,
	DrawIndirectBufferBinding,
	ElementArrayBufferBinding,
	PixelPackBufferBinding,
	PixelUnpackBufferBinding,
	ShaderStorageBufferBinding,
	TextureBindingBuffer,
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
	MapPersistentBit,
	MapCoherentBit
};

//=============================================================================
// IndexBuffer enum
//=============================================================================

enum class IndexType : uint8_t
{
	Uint8,
	Uint16,
	Uint32
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
	LinesAdjacency,
	LineStripAdjacency,
	TrianglesAdjacency,
	TriangleStripAdjacency,
	Patches
};

//=============================================================================
// Texture enum
//=============================================================================

enum class TextureType : uint8_t // TODO: использовать
{
	Texture1D,
	Texture2D,
	Texture3D,
	TextureRectangle,
	Texture1DArray,
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
#if !defined(__EMSCRIPTEN__) // not support in webGL
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
	FramebufferIncompleteDrawBuffer,
	FramebufferIncompleteReadBuffer,
	FramebufferUnsupported,
	FramebufferIncompleteMultisample,
	FramebufferIncompleteLayerTargets
};

enum class FramebufferBinding : uint8_t
{
	ReadFramebufferBinding,
	DrawFramebufferBinding,
	FramebufferBinding
};