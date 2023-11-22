#pragma once

class Image final
{
public:
	enum PixelFormat
	{
		None = 0,
		R_U8,
		RG_U8,
		RGB_U8,
		RGBA_U8,
	};

	Image() = default;
	Image(Image&&) = delete;
	Image(const Image&) = delete;
	Image(const uint8_t* data, size_t dataSize);
	Image(const std::string& fileName);
	~Image();

	Image& operator=(Image&&) = delete;
	Image& operator=(const Image&) = delete;

	void Create(Image::PixelFormat imageFormat, int width, int height, uint8_t* pixelData);
	bool LoadFromMemory(const uint8_t* data, size_t dataSize);
	bool LoadFromFile(const std::string& fileName);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	uint8_t* GetTexels() { return m_pixelData; }
	Image::PixelFormat GetPixelFormat() const { return m_imageFormat; }
	bool HasTransparency() const { return m_hasTransparency; }
	bool IsValid() const { return m_pixelData != nullptr; }

private:
	bool isPixelsHaveTransparency() const;
	int m_width = 0;                                  // Image base width
	int m_height = 0;                                 // Image base height
	uint8_t* m_pixelData = nullptr;                   // Image raw data
	Image::PixelFormat m_imageFormat = Image::PixelFormat::RGBA_U8; // Data format 
	int m_mipmaps = 1;                                // Mipmap levels, 1 by default
	bool m_hasTransparency = false;

	enum sourceImage
	{
		stb,
		custom
	} m_source = stb;
};
using ImageRef = std::shared_ptr<Image>;