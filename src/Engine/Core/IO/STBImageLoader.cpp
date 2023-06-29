#include "stdafx.h"
#include "STBImageLoader.h"
//-----------------------------------------------------------------------------
STBImageLoader::STBImageLoader(const char* fileName)
{
	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desiredСhannels = STBI_default;
	int nrChannels = 0;
	pixelData = stbi_load(fileName, &width, &height, &nrChannels, desiredСhannels);
	if (nrChannels == STBI_grey)            imageFormat = ImagePixelFormat::R_U8;
	else if (nrChannels == STBI_grey_alpha) imageFormat = ImagePixelFormat::RG_U8;
	else if (nrChannels == STBI_rgb)        imageFormat = ImagePixelFormat::RGB_U8;
	else if (nrChannels == STBI_rgb_alpha)  imageFormat = ImagePixelFormat::RGBA_U8;

	if( !pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || width == 0 || height == 0 )
		pixelData = nullptr;
}
//-----------------------------------------------------------------------------
STBImageLoader::~STBImageLoader()
{
	stbi_image_free((void*)pixelData);
}
//-----------------------------------------------------------------------------