﻿#include "stdafx.h"
#include "FileSystem.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
std::optional<std::vector<uint8_t>> FileSystem::FileToMemory(const std::string& fileName, unsigned int* bytesRead)
{
	if (fileName.empty())
	{
		LogError("File name provided is not valid");
		return std::nullopt;
	}

	FILE* file = nullptr;
	errno_t err;
	err = fopen_s(&file, fileName.c_str(), "rb");
	if (err != 0 || !file)
	{
		LogError("Failed to open file '" + fileName + "'");
		return std::nullopt;
	}

	if (fseek(file, 0L, SEEK_END) != 0)
	{
		LogError("Unable to seek file '" + fileName + "'");
		fclose(file);
		return std::nullopt;
	}

	const long fileLen = ftell(file);
	if (fileLen <= 0)
	{
		LogError("Failed to read file '" + fileName + "'");
		fclose(file);
		return std::nullopt;
	}

	if (fseek(file, 0, SEEK_SET) != 0)
	{
		LogError("Unable to seek file '" + fileName + "'");
		fclose(file);
		return std::nullopt;
	}

	std::vector<uint8_t> contents(static_cast<size_t>(fileLen + 1));
	if (contents.empty() || contents.size() <= 1)
	{
		LogError("Memory error!");
		fclose(file);
		return std::nullopt;
	}

	const size_t count = fread(contents.data(), sizeof(unsigned char), static_cast<size_t>(fileLen), file);
	if (count == 0 || ferror(file))
	{
		LogError("Read error");
		fclose(file);
		return std::nullopt;
	}
	contents[static_cast<size_t>(fileLen)] = 0;

	*bytesRead = static_cast<unsigned int>(count);

	fclose(file);
	return contents;
}
//-----------------------------------------------------------------------------
const char* FileSystem::GetFileExtension(const char* fileName)
{
	const char* dot = strrchr(fileName, '.');
	if (!dot || dot == fileName) return nullptr;
	return dot;
}
//-----------------------------------------------------------------------------
const char* FileSystem::GetFileName(const char* filePath)
{
	const char* latestMatch = nullptr;
	for (; filePath = strpbrk(filePath, "\\/"), filePath != nullptr; latestMatch = filePath++) {}
	const char* fileName = latestMatch;
	if (!fileName) return filePath;
	return fileName + 1;
}
//-----------------------------------------------------------------------------