#pragma once

namespace Capabilities
{
	extern uint32_t maximumTextureDimension;  // Maximum texture dimension (usually 2048, 4096, 8192 or 16384)
	extern uint32_t maximumTextureUnitCount;
	extern uint32_t maximumUniformBufferSize; // Maximum uniform buffer (UBO) size in bytes (usually at least 4096 *16 bytes, in case there's no support for uniform buffer it's 0)
}