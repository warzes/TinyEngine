#include "stdafx.h"
#include "ImageLoader.h"
//-----------------------------------------------------------------------------
ImageLoader::ImageLoader(const char* fileName)
{
	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desiredСhannels = STBI_default;
	int nrChannels = 0;
	m_pixelData = stbi_load(fileName, &m_width, &m_height, &nrChannels, desiredСhannels);
	if (nrChannels == STBI_grey)            m_imageFormat = ImageFormat::R_U8;
	else if (nrChannels == STBI_grey_alpha) m_imageFormat = ImageFormat::RG_U8;
	else if (nrChannels == STBI_rgb)        m_imageFormat = ImageFormat::RGB_U8;
	else if (nrChannels == STBI_rgb_alpha)  m_imageFormat = ImageFormat::RGBA_U8;

	if( !m_pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || m_width == 0 || m_height == 0 )
		m_pixelData = nullptr;

	m_hasTransparency = isPixelsHaveTransparency();
}
//-----------------------------------------------------------------------------
ImageLoader::~ImageLoader()
{
	stbi_image_free((void*)m_pixelData);
}
//-----------------------------------------------------------------------------
bool ImageLoader::isPixelsHaveTransparency() const
{
	if (!m_pixelData) return false;
	if (m_imageFormat != ImageFormat::RGBA_U8) return false;

	const size_t sizeData = m_width * m_height * 4;
	for (std::size_t i = 3; i < sizeData; i += 4)
	{
		if (m_pixelData[i] < 255)
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------------