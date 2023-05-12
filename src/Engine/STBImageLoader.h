#pragma once

enum class ImageFormat : uint8_t
{
	None = 0,
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,
};

class STBImageLoader
{
public:
	STBImageLoader(const char* fileName);
	~STBImageLoader();

	int width = 0;
	int height = 0;
	int nrChannels = 0;
	stbi_uc* pixelData = nullptr;
	ImageFormat imageFormat = ImageFormat::RGBA_U8;

	bool isValid = false;
};