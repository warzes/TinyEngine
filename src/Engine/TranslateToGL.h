#pragma once

#include "RenderResource.h"
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(ImageFormat format)
{
	switch( format )
	{
	case ImageFormat::R8:              return GL_R8;
	case ImageFormat::RG8:             return GL_RG8;
	case ImageFormat::RGBA8:           return GL_RGBA8;
	case ImageFormat::A8:              return GL_ALPHA;
	case ImageFormat::R16:             return GL_R16;
	case ImageFormat::RG16:            return GL_RG16;
	case ImageFormat::RGBA16:          return GL_RGBA16;
	case ImageFormat::R16F:            return GL_R16F;
	case ImageFormat::RG16F:           return GL_RG16F;
	case ImageFormat::RGBA16F:         return GL_RGBA16F;
	case ImageFormat::R32F:            return GL_R32F;
	case ImageFormat::RG32F:           return GL_RG32F;
	case ImageFormat::RGB32F:          return GL_RGB32F;
	case ImageFormat::RGBA32F:         return GL_RGBA32F;
	case ImageFormat::R32U:            return GL_R32UI;
	case ImageFormat::RG32U:           return GL_RG32UI;
	case ImageFormat::RGBA32U:         return GL_RGBA32UI;
	case ImageFormat::D16:             return GL_DEPTH_COMPONENT16;
	case ImageFormat::D32:             return GL_DEPTH_COMPONENT32;
	case ImageFormat::D24S8:           return GL_DEPTH24_STENCIL8;
	case ImageFormat::DXT1:            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case ImageFormat::DXT3:            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case ImageFormat::DXT5:            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	case ImageFormat::None:
	default: break;
	}
	assert(false && "Unknown ImageFormat");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(ComparisonFunc func)
{
	switch (func)
	{
	case ComparisonFunc::Never:    return GL_NEVER;
	case ComparisonFunc::Equal:    return GL_EQUAL;
	case ComparisonFunc::Less:     return GL_LESS;
	case ComparisonFunc::LEqual:   return GL_LEQUAL;
	case ComparisonFunc::Greater:  return GL_GREATER;
	case ComparisonFunc::GEqual:   return GL_GEQUAL;
	case ComparisonFunc::NotEqual: return GL_NOTEQUAL;
	case ComparisonFunc::Always:   return GL_ALWAYS;

	default: break;
	}
	assert(false && "Unknown ComparisonFunc");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(StencilOp op)
{
	switch (op)
	{
	case StencilOp::Keep:     return GL_KEEP;
	case StencilOp::Zero:     return GL_ZERO;
	case StencilOp::Replace:  return GL_REPLACE;
	case StencilOp::IncrSat:  return GL_INCR;
	case StencilOp::DecrSat:  return GL_DECR;
	case StencilOp::Invert:   return GL_INVERT;
	case StencilOp::IncrWrap: return GL_INCR_WRAP;
	case StencilOp::DecrWrap: return GL_DECR_WRAP;
	default: break;
	}
	assert(false && "Unknown StencilOp");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BlendOp op)
{
	switch( op )
	{
	case BlendOp::Add:             return GL_FUNC_ADD;
	case BlendOp::Subrtact:        return GL_FUNC_SUBTRACT;
	case BlendOp::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
	case BlendOp::Min:             return GL_MIN;
	case BlendOp::Max:             return GL_MAX;
	default: break;
	}
	assert(false && "Unknown BlendOp");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BlendFactor func)
{
	switch( func )
	{
	case BlendFactor::Zero:                  return GL_ZERO;
	case BlendFactor::One:                   return GL_ONE;
	case BlendFactor::SrcColor:              return GL_SRC_COLOR;
	case BlendFactor::OneMinusSrcColor:      return GL_ONE_MINUS_SRC_COLOR;
	case BlendFactor::DstColor:              return GL_DST_COLOR;
	case BlendFactor::OneMinusDstColor:      return GL_ONE_MINUS_DST_COLOR;
	case BlendFactor::SrcAlpha:              return GL_SRC_ALPHA;
	case BlendFactor::OneMinusSrcAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::DstAlpha:              return GL_DST_ALPHA;
	case BlendFactor::OneMinusDstAlpha:      return GL_ONE_MINUS_DST_ALPHA;
	case BlendFactor::ConstantColor:         return GL_CONSTANT_COLOR;
	case BlendFactor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
	case BlendFactor::ConstantAlpha:         return GL_CONSTANT_ALPHA;
	case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
	case BlendFactor::SrcAlphaSaturate:      return GL_SRC_ALPHA_SATURATE;
	case BlendFactor::Src1Color:             return GL_SRC1_COLOR;
	case BlendFactor::OneMinusSrc1Color:     return GL_ONE_MINUS_SRC1_COLOR;
	case BlendFactor::Src1Alpha:             return GL_SRC1_ALPHA;
	case BlendFactor::OneMinusSrc1Alpha:     return GL_ONE_MINUS_SRC1_ALPHA;
	default: break;
	}
	assert(false && "Unknown BlendFactor");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(RasterizerCullMode face)
{
	switch( face )
	{
	case RasterizerCullMode::Back:         return GL_BACK;
	case RasterizerCullMode::Front:        return GL_FRONT;
	case RasterizerCullMode::FrontAndBack: return GL_FRONT_AND_BACK;
	default: break;
	}
	assert(false && "Unknown RasterCullMode");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(FaceOrientation orientation)
{
	switch( orientation )
	{
	case FaceOrientation::Cw:  return GL_CW;
	case FaceOrientation::Ccw: return GL_CCW;
	default: break;
	}
	assert(false && "Unknown FaceOrientation");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(ShaderType usage)
{
	switch( usage )
	{
	case ShaderType::Vertex:      return GL_VERTEX_SHADER;
	case ShaderType::Fragment:    return GL_FRAGMENT_SHADER;
	case ShaderType::Geometry:    return GL_GEOMETRY_SHADER;
	case ShaderType::Compute:     return GL_COMPUTE_SHADER;
	case ShaderType::TessControl: return GL_TESS_CONTROL_SHADER;
	case ShaderType::Evaluation:  return GL_TESS_EVALUATION_SHADER;
	default: break;
	}
	assert(false && "Unknown ShaderType");
	return 0;
}
//-----------------------------------------------------------------------------
inline std::string ConvertToStr(ShaderType usage)
{
	switch( usage )
	{
	case ShaderType::Vertex:      return "Vertex";
	case ShaderType::Fragment:    return "Fragment";
	case ShaderType::Geometry:    return "Geometry";
	case ShaderType::Compute:     return "Compute";
	case ShaderType::TessControl: return "TessControl";
	case ShaderType::Evaluation:  return "Evaluation";
	default: break;
	}
	assert(false && "Unknown ShaderType");
	return "";
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BufferType type)
{
	switch( type )
	{
	case BufferType::ArrayBuffer:             return GL_ARRAY_BUFFER;
	case BufferType::AtomicCounterBuffer:     return GL_ATOMIC_COUNTER_BUFFER;
	case BufferType::CopyReadBuffer:          return GL_COPY_READ_BUFFER;
	case BufferType::CopyWriteBuffer:         return GL_COPY_WRITE_BUFFER;
	case BufferType::DispatchIndirectBuffer:  return GL_DISPATCH_INDIRECT_BUFFER;
	case BufferType::DrawIndirectBuffer:      return GL_DRAW_INDIRECT_BUFFER;
	case BufferType::ElementArrayBuffer:      return GL_ELEMENT_ARRAY_BUFFER;
	case BufferType::PixelPackBuffer:         return GL_PIXEL_PACK_BUFFER;
	case BufferType::PixelUnpackBuffer:       return GL_PIXEL_UNPACK_BUFFER;
	case BufferType::ShaderStorageBuffer:     return GL_SHADER_STORAGE_BUFFER;
	case BufferType::TextureBuffer:           return GL_TEXTURE_BUFFER;
	case BufferType::TransformFeedbackBuffer: return GL_TRANSFORM_FEEDBACK_BUFFER;
	case BufferType::UniformBuffer:           return GL_UNIFORM_BUFFER;
	default: break;
	}
	assert(false && "Unknown BufferType");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BufferUsage usage)
{
	switch( usage )
	{
	case BufferUsage::StaticDraw:  return GL_STATIC_DRAW;
	case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case BufferUsage::StreamDraw:  return GL_STREAM_DRAW;
	case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
	case BufferUsage::StaticRead:  return GL_STATIC_READ;
	case BufferUsage::StreamRead:  return GL_STREAM_READ;
	case BufferUsage::StaticCopy:  return GL_STATIC_COPY;
	case BufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
	case BufferUsage::StreamCopy:  return GL_STREAM_COPY;
	default: break;
	}	
	assert(false && "Unknown BufferUsage");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BufferMapAccess access)
{
	switch( access )
	{
	case BufferMapAccess::Read:      return GL_READ_ONLY;
	case BufferMapAccess::Write:     return GL_WRITE_ONLY;
	case BufferMapAccess::ReadWrite: return GL_READ_WRITE;
	default: break;
	}
	assert(false && "Unknown BufferMapAccess");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BufferBinding binding)
{
	switch( binding )
	{
	case BufferBinding::ArrayBufferBinding:             return GL_ARRAY_BUFFER_BINDING;
	case BufferBinding::AtomicCounterBufferBinding:     return GL_ATOMIC_COUNTER_BUFFER_BINDING;
	case BufferBinding::CopyReadBuffer:                 return GL_COPY_READ_BUFFER;
	case BufferBinding::CopyWriteBuffer:                return GL_COPY_WRITE_BUFFER;
	case BufferBinding::DispatchIndirectBufferBinding:  return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
	case BufferBinding::DrawIndirectBufferBinding:      return GL_DRAW_INDIRECT_BUFFER_BINDING;
	case BufferBinding::ElementArrayBufferBinding:      return GL_ELEMENT_ARRAY_BUFFER_BINDING;
	case BufferBinding::PixelPackBufferBinding:         return GL_PIXEL_PACK_BUFFER_BINDING;
	case BufferBinding::PixelUnpackBufferBinding:       return GL_PIXEL_UNPACK_BUFFER_BINDING;
	case BufferBinding::ShaderStorageBufferBinding:     return GL_SHADER_STORAGE_BUFFER_BINDING;
	case BufferBinding::TextureBindingBuffer:           return GL_TEXTURE_BINDING_BUFFER;
	case BufferBinding::TransformFeedbackBufferBinding: return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
	case BufferBinding::UniformBufferBinding:           return GL_UNIFORM_BUFFER_BINDING;
	default: break;
	}
	assert(false && "Unknown BufferBinding");
	return 0;
}
//-----------------------------------------------------------------------------
inline BufferBinding GetBindingTarget(BufferType type)
{
	BufferBinding target;
	switch( type )
	{
	case BufferType::ArrayBuffer:             target = BufferBinding::ArrayBufferBinding; break;
	case BufferType::AtomicCounterBuffer:     target = BufferBinding::AtomicCounterBufferBinding; break;
	case BufferType::CopyReadBuffer:          target = BufferBinding::CopyReadBuffer; break;
	case BufferType::CopyWriteBuffer:         target = BufferBinding::CopyWriteBuffer; break;
	case BufferType::DispatchIndirectBuffer:  target = BufferBinding::DispatchIndirectBufferBinding; break;
	case BufferType::DrawIndirectBuffer:      target = BufferBinding::DrawIndirectBufferBinding; break;
	case BufferType::ElementArrayBuffer:      target = BufferBinding::ElementArrayBufferBinding; break;
	case BufferType::PixelPackBuffer:         target = BufferBinding::PixelPackBufferBinding; break;
	case BufferType::PixelUnpackBuffer:       target = BufferBinding::PixelUnpackBufferBinding; break;
	case BufferType::ShaderStorageBuffer:     target = BufferBinding::ShaderStorageBufferBinding; break;
	case BufferType::TextureBuffer:           target = BufferBinding::TextureBindingBuffer; break;
	case BufferType::TransformFeedbackBuffer: target = BufferBinding::TransformFeedbackBufferBinding; break;
	case BufferType::UniformBuffer:           target = BufferBinding::UniformBufferBinding; break;
	default: break;
	}
	assert(false && "Unknown BufferType");
	return {};
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(BufferMapAccessFlags accessFlags)
{
	switch( accessFlags )
	{
	case BufferMapAccessFlags::MapReadBit:             return GL_MAP_READ_BIT;
	case BufferMapAccessFlags::MapWriteBit:            return GL_MAP_WRITE_BIT;
	case BufferMapAccessFlags::MapInvalidateRangeBit:  return GL_MAP_INVALIDATE_RANGE_BIT;
	case BufferMapAccessFlags::MapInvalidateBufferBit: return GL_MAP_INVALIDATE_BUFFER_BIT;
	case BufferMapAccessFlags::MapFlushExplicitBit:    return GL_MAP_FLUSH_EXPLICIT_BIT;
	case BufferMapAccessFlags::MapUnsynchronizedBit:   return GL_MAP_UNSYNCHRONIZED_BIT;
	case BufferMapAccessFlags::MapPersistentBit:       return GL_MAP_PERSISTENT_BIT;
	case BufferMapAccessFlags::MapCoherentBit:         return GL_MAP_COHERENT_BIT;
	default: break;
	}
	assert(false && "Unknown BufferMapAccessFlags");
	return 0;
}
//-----------------------------------------------------------------------------
inline constexpr unsigned SizeIndexType(IndexType format)
{
	switch( format )
	{
	case IndexType::Uint8:  return sizeof(uint8_t);
	case IndexType::Uint16: return sizeof(uint16_t);
	case IndexType::Uint32: return sizeof(uint32_t);
	default: break;
	}
	assert(false && "Unknown IndexType");
	return 0;
}
//-----------------------------------------------------------------------------
inline constexpr unsigned SizeIndexType(unsigned size)
{
	switch( size )
	{
	case 1: return GL_UNSIGNED_BYTE;
	case 2: return GL_UNSIGNED_SHORT;
	case 4: return GL_UNSIGNED_INT;
	default: break;
	}
	assert(false && "Unknown size");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(IndexType type) // TODO: использовать
{
	switch( type )
	{
	case IndexType::Uint8:  return GL_UNSIGNED_BYTE;
	case IndexType::Uint16: return GL_UNSIGNED_SHORT;
	case IndexType::Uint32: return GL_UNSIGNED_INT;
	default: break;
	}
	assert(false && "Unknown IndexType");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(PrimitiveTopology topology)
{
	switch( topology )
	{
	case PrimitiveTopology::Points:                 return GL_POINTS;
	case PrimitiveTopology::Lines:                  return GL_LINES;
	case PrimitiveTopology::LineStrip:              return GL_LINE_STRIP;
	case PrimitiveTopology::LineLoop:               return GL_LINE_LOOP;
	case PrimitiveTopology::Triangles:              return GL_TRIANGLES;
	case PrimitiveTopology::TriangleStrip:          return GL_TRIANGLE_STRIP;
	case PrimitiveTopology::TriangleFan:            return GL_TRIANGLE_FAN;
	case PrimitiveTopology::LinesAdjacency:         return GL_LINES_ADJACENCY;
	case PrimitiveTopology::LineStripAdjacency:     return GL_LINE_STRIP_ADJACENCY;
	case PrimitiveTopology::TrianglesAdjacency:     return GL_TRIANGLES_ADJACENCY;
	case PrimitiveTopology::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY;
	case PrimitiveTopology::Patches:                return GL_PATCHES;
	default: break;
	}
	assert(false && "Unknown PrimitiveTopology");
	return 0;
}
//-----------------------------------------------------------------------------
inline constexpr GLint TranslateToGL(TextureType type)
{
	switch( type )
	{
	case TextureType::Texture1D:        return GL_TEXTURE_1D;
	case TextureType::Texture2D:        return GL_TEXTURE_2D;
	case TextureType::Texture3D:        return GL_TEXTURE_3D;
	case TextureType::TextureRectangle: return GL_TEXTURE_RECTANGLE;
	case TextureType::Texture1DArray:   return GL_TEXTURE_1D_ARRAY;
	case TextureType::Texture2DArray:   return GL_TEXTURE_2D_ARRAY;
	case TextureType::TextureCubeMap:   return GL_TEXTURE_CUBE_MAP;
	default: break;
	}
	assert(false && "Unknown TextureType");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(TextureMinFilter filter)
{
	switch( filter )
	{
	case TextureMinFilter::Nearest:              return GL_NEAREST;
	case TextureMinFilter::Linear:               return GL_LINEAR;
	case TextureMinFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureMinFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
	case TextureMinFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
	case TextureMinFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
	default: break;
	}
	assert(false && "Unknown TextureMinFilter");
	return 0;
}
//-----------------------------------------------------------------------------
inline constexpr GLint TranslateToGL(TextureMagFilter filter)
{
	switch( filter )
	{
	case TextureMagFilter::Nearest: return GL_NEAREST;
	case TextureMagFilter::Linear:  return GL_LINEAR;
	default: break;
	}
	assert(false && "Unknown TextureMagFilter");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(TextureAddressMode wrapMode)
{
	switch( wrapMode )
	{
	case TextureAddressMode::Repeat:         return GL_REPEAT;
	case TextureAddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureAddressMode::ClampToEdge:    return GL_CLAMP_TO_EDGE;
#if !defined(__EMSCRIPTEN__)
	case TextureAddressMode::ClampToBorder:  return GL_CLAMP_TO_BORDER;
#endif
	default: break;
	}
	assert(false && "Unknown TextureAddressMode");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(TextureCubeTarget target)
{
	switch( target )
	{
	case TextureCubeTarget::TextureCubeMapPositiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	case TextureCubeTarget::TextureCubeMapNegativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	case TextureCubeTarget::TextureCubeMapPositiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	case TextureCubeTarget::TextureCubeMapNegativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	case TextureCubeTarget::TextureCubeMapPositiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	case TextureCubeTarget::TextureCubeMapNegativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	default: break;
	}
	assert(false && "Unknown TextureCubeTarget");
	return 0;
}
//-----------------------------------------------------------------------------
inline bool GetTextureFormatType(TexelsFormat inFormat, GLenum textureType, GLenum& format, GLint& internalFormat, GLenum& oglType)
{
	if( inFormat == TexelsFormat::R_U8 )
	{
		format = GL_RED;
		internalFormat = GL_R8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == TexelsFormat::RG_U8 )
	{
#if defined(_WIN32)
		format = GL_RG;
		internalFormat = GL_RG8;
		oglType = GL_UNSIGNED_BYTE;
		const GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
		glTexParameteriv(textureType, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask); // TODO: могут быть проблемы с браузерами, тогда только грузить stb с указанием нужного формата
#endif // _WIN32
#if defined(__EMSCRIPTEN__)
		LogFatal("TexelsFormat::RG_U8 not support in web platform");
		return false;
#endif
	}
	else if( inFormat == TexelsFormat::RGB_U8 )
	{
		format = GL_RGB;
		internalFormat = GL_RGB;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == TexelsFormat::RGBA_U8 )
	{
		format = GL_RGBA;
		internalFormat = GL_RGBA8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == TexelsFormat::RG_F32 )
	{
		format = GL_RG;
		internalFormat = GL_RG32F;
		oglType = GL_FLOAT;
	}
	else if( inFormat == TexelsFormat::Depth_U16 )
	{
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT16;
		oglType = GL_UNSIGNED_SHORT;
	}
	else if( inFormat == TexelsFormat::DepthStencil_U16 )
	{
		format = GL_DEPTH_STENCIL;
		internalFormat = GL_DEPTH24_STENCIL8;
		oglType = GL_UNSIGNED_SHORT;
	}
	else if( inFormat == TexelsFormat::Depth_U24 )
	{
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT24;
		oglType = GL_UNSIGNED_INT;
	}
	else if( inFormat == TexelsFormat::DepthStencil_U24 )
	{
		format = GL_DEPTH_STENCIL;
		internalFormat = GL_DEPTH24_STENCIL8;
		oglType = GL_UNSIGNED_INT;
	}
	else
	{
		//Error("unknown texture format");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(FramebufferAttachment attachment)
{
	switch( attachment )
	{
	case FramebufferAttachment::ColorAttachment0:       return GL_COLOR_ATTACHMENT0;
	case FramebufferAttachment::ColorAttachment1:       return GL_COLOR_ATTACHMENT1;
	case FramebufferAttachment::ColorAttachment2:       return GL_COLOR_ATTACHMENT2;
	case FramebufferAttachment::ColorAttachment3:       return GL_COLOR_ATTACHMENT3;
	case FramebufferAttachment::ColorAttachment4:       return GL_COLOR_ATTACHMENT4;
	case FramebufferAttachment::ColorAttachment5:       return GL_COLOR_ATTACHMENT5;
	case FramebufferAttachment::ColorAttachment6:       return GL_COLOR_ATTACHMENT6;
	case FramebufferAttachment::ColorAttachment7:       return GL_COLOR_ATTACHMENT7;
	case FramebufferAttachment::ColorAttachment8:       return GL_COLOR_ATTACHMENT8;
	case FramebufferAttachment::ColorAttachment9:       return GL_COLOR_ATTACHMENT9;
	case FramebufferAttachment::ColorAttachment10:      return GL_COLOR_ATTACHMENT10;
	case FramebufferAttachment::ColorAttachment11:      return GL_COLOR_ATTACHMENT11;
	case FramebufferAttachment::ColorAttachment12:      return GL_COLOR_ATTACHMENT12;
	case FramebufferAttachment::ColorAttachment13:      return GL_COLOR_ATTACHMENT13;
	case FramebufferAttachment::ColorAttachment14:      return GL_COLOR_ATTACHMENT14;
	case FramebufferAttachment::ColorAttachment15:      return GL_COLOR_ATTACHMENT15;
	case FramebufferAttachment::DepthAttachment:        return GL_DEPTH_ATTACHMENT;
	case FramebufferAttachment::StencilAttachment:      return GL_STENCIL_ATTACHMENT;
	case FramebufferAttachment::DepthStencilAttachment: return GL_DEPTH_STENCIL_ATTACHMENT;
	default: break;
	}
	assert(false && "Unknown TextureAddressMode");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(FramebufferType type)
{
	switch( type )
	{
	case FramebufferType::ReadFramebuffer: return GL_READ_FRAMEBUFFER;
	case FramebufferType::DrawFramebuffer: return GL_DRAW_FRAMEBUFFER;
	case FramebufferType::Framebuffer:     return GL_FRAMEBUFFER;
	default: break;
	}
	assert(false && "Unknown FramebufferType");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(FramebufferStatus status)
{
	switch( status )
	{
	case FramebufferStatus::FramebufferComplete:                    return GL_FRAMEBUFFER_COMPLETE;
	case FramebufferStatus::FramebufferUndefined:                   return GL_FRAMEBUFFER_UNDEFINED;
	case FramebufferStatus::FramebufferIncompleteAttachment:        return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
	case FramebufferStatus::FramebufferIncompleteMissingAttachment: return GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
	case FramebufferStatus::FramebufferIncompleteDrawBuffer:        return GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER;
	case FramebufferStatus::FramebufferIncompleteReadBuffer:        return GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER;
	case FramebufferStatus::FramebufferUnsupported:                 return GL_FRAMEBUFFER_UNSUPPORTED;
	case FramebufferStatus::FramebufferIncompleteMultisample:       return GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE;
	case FramebufferStatus::FramebufferIncompleteLayerTargets:      return GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;
	default: break;
	}
	assert(false && "Unknown FramebufferStatus");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(FramebufferBinding binding)
{
	switch( binding )
	{
	case FramebufferBinding::ReadFramebufferBinding: return GL_READ_FRAMEBUFFER_BINDING;
	case FramebufferBinding::DrawFramebufferBinding: return GL_DRAW_FRAMEBUFFER_BINDING;
	case FramebufferBinding::FramebufferBinding:     return GL_FRAMEBUFFER_BINDING;
	default: break;
	}
	assert(false && "Unknown FramebufferBinding");
	return 0;
}
//-----------------------------------------------------------------------------
inline FramebufferBinding GetBindingTarget(FramebufferType type)
{
	switch( type )
	{
	case FramebufferType::ReadFramebuffer: return FramebufferBinding::ReadFramebufferBinding;
	case FramebufferType::DrawFramebuffer: return FramebufferBinding::DrawFramebufferBinding;
	case FramebufferType::Framebuffer:     return FramebufferBinding::FramebufferBinding;
	default: break;
	}
	assert(false && "Unknown FramebufferType");
	return {};
}
//-----------------------------------------------------------------------------
inline GLenum GetAttributeType(GLenum type)
{
	switch( type )
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
		return GL_FLOAT;
	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return GL_INT;
	}
	assert(false && "Unknown active attribute type!");
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint GetAttributeSize(GLenum type)
{
	switch( type )
	{
	case GL_FLOAT:
	case GL_INT:
		return 1;
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
		return 2;
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
		return 3;
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
		return 4;
	}

	assert(false && "Unknown active attribute type!");
	return 0;
}
//-----------------------------------------------------------------------------