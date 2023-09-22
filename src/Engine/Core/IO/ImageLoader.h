#pragma once

class ImageLoader final
{
public:
	enum ImageFormat
	{
		None = 0,
		R_U8,
		RG_U8,
		RGB_U8,
		RGBA_U8,
	};

	ImageLoader() = delete;
	ImageLoader(ImageLoader&&) = delete;
	ImageLoader(const ImageLoader&) = delete;
	ImageLoader(const char* fileName);
	~ImageLoader();

	ImageLoader& operator=(ImageLoader&&) = delete;
	ImageLoader& operator=(const ImageLoader&) = delete;

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	uint8_t* GetTexels() { return m_pixelData; }
	ImageFormat GetImageFormat() const { return m_imageFormat; }
	bool HasTransparency() const { return m_hasTransparency; }

private:
	bool isPixelsHaveTransparency() const;
	int m_width = 0;
	int m_height = 0;
	uint8_t* m_pixelData = nullptr;
	ImageFormat m_imageFormat = RGBA_U8;
	bool m_hasTransparency = false;
};