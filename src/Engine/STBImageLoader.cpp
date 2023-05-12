#include "stdafx.h"
#include "STBImageLoader.h"
//-----------------------------------------------------------------------------
STBImageLoader::STBImageLoader(const char* fileName)
{
	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desiredСhannels = STBI_default;
	pixelData = stbi_load(fileName, &width, &height, &nrChannels, desiredСhannels);
	if( !pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || width == 0 || height == 0 )
	{
		isValid = false;
		return;
	}

	if( nrChannels == STBI_grey ) imageFormat = ImageFormat::R_U8;
	else if( nrChannels == STBI_grey_alpha ) imageFormat = ImageFormat::RG_U8;
	else if( nrChannels == STBI_rgb ) imageFormat = ImageFormat::RGB_U8;
	else if( nrChannels == STBI_rgb_alpha ) imageFormat = ImageFormat::RGBA_U8;

	isValid = true;
}
//-----------------------------------------------------------------------------
STBImageLoader::~STBImageLoader()
{
	stbi_image_free((void*)pixelData);
}
//-----------------------------------------------------------------------------