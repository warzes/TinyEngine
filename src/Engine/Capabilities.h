#pragma once

namespace Capabilities
{
	uint32_t maximumTextureDimension;  // Maximum texture dimension (usually 2048, 4096, 8192 or 16384)
	uint32_t maximumTextureUnitCount;
	uint32_t maximumUniformBufferSize; // Maximum uniform buffer (UBO) size in bytes (usually at least 4096 *16 bytes, in case there's no support for uniform buffer it's 0)
}