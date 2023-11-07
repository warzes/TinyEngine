#include "stdafx.h"
#include "Image.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
inline ImageLoaderFormat convertSTBToEngine(int nrChannels)
{
	if (nrChannels == STBI_grey) return            ImageLoaderFormat::R_U8;
	else if (nrChannels == STBI_grey_alpha) return ImageLoaderFormat::RG_U8;
	else if (nrChannels == STBI_rgb) return        ImageLoaderFormat::RGB_U8;
	else if (nrChannels == STBI_rgb_alpha) return  ImageLoaderFormat::RGBA_U8;
	else
	{
		LogFatal("unknown nrChannels");
		return ImageLoaderFormat::RGBA_U8;
	}
}
//-----------------------------------------------------------------------------
Image::Image(const uint8_t* data, size_t dataSize)
{
	LoadFromMemory(data, dataSize);
}
//-----------------------------------------------------------------------------
Image::Image(const std::string& fileName)
{
	LoadFromFile(fileName);
}
//-----------------------------------------------------------------------------
Image::~Image()
{
	if (m_source == stb) stbi_image_free((void*)m_pixelData);
}
//-----------------------------------------------------------------------------
void Image::Create(ImageLoaderFormat imageFormat, int width, int height, uint8_t* pixelData)
{
	m_imageFormat = imageFormat;
	m_width = width;
	m_height = height;
	m_pixelData = pixelData;
	m_source = custom;
}
//-----------------------------------------------------------------------------
bool Image::LoadFromMemory(const uint8_t* data, size_t dataSize)
{
	if (!data || dataSize == 0)
	{
		LogError("IMAGE: Failed to load image data");
		return false;
	}

	const int desiredСhannels = STBI_default;
	int nrChannels = 0;
	m_pixelData = stbi_load_from_memory(data, dataSize, &m_width, &m_height, &nrChannels, desiredСhannels);
	m_imageFormat = convertSTBToEngine(nrChannels);

	if (!m_pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || m_width == 0 || m_height == 0)
	{
		m_pixelData = nullptr;
		return false;
	}
	m_source = stb;
	m_mipmaps = 1;
	m_hasTransparency = isPixelsHaveTransparency();
	return true;
}
//-----------------------------------------------------------------------------
bool Image::LoadFromFile(const std::string& fileName)
{
	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desiredСhannels = STBI_default;
	int nrChannels = 0;
	m_pixelData = stbi_load(fileName.c_str(), &m_width, &m_height, &nrChannels, desiredСhannels);
	m_imageFormat = convertSTBToEngine(nrChannels);

	if (!m_pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || m_width == 0 || m_height == 0)
	{
		m_pixelData = nullptr;
		return false;
	}

	m_source = stb;
	m_mipmaps = 1;
	m_hasTransparency = isPixelsHaveTransparency();
	return true;
}
//-----------------------------------------------------------------------------
bool Image::isPixelsHaveTransparency() const
{
	if (!m_pixelData) return false;
	if (m_imageFormat != ImageLoaderFormat::RGBA_U8) return false;

	const size_t sizeData = m_width * m_height * 4;
	for (std::size_t i = 3; i < sizeData; i += 4)
	{
		if (m_pixelData[i] < 255)
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------------