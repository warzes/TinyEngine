#pragma once

enum class ImagePixelFormat : uint8_t
{
	None = 0,
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,
};

class STBImageLoader final
{
public:
	STBImageLoader() = delete;
	STBImageLoader(STBImageLoader&&) = delete;
	STBImageLoader(const STBImageLoader&) = delete;
	STBImageLoader(const char* fileName);
	~STBImageLoader();

	STBImageLoader& operator=(STBImageLoader&&) = delete;
	STBImageLoader& operator=(const STBImageLoader&) = delete;

	int width = 0;
	int height = 0;
	stbi_uc* pixelData = nullptr;
	ImagePixelFormat imageFormat = ImagePixelFormat::RGBA_U8;
};