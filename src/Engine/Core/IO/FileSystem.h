﻿#pragma once

namespace FileSystem
{
	// Returns true if the file exists, false otherwise.
	[[nodiscard]] bool Exists(const std::string& path) noexcept;


#if !PLATFORM_EMSCRIPTEN // TODO:
	[[nodiscard]] std::optional<std::vector<uint8_t>> FileToMemory(const std::string& fileName, unsigned int* bytesRead = nullptr);
#endif

	// TODO: убрать из кода заменив на std::filesystem
	
	// Get pointer to extension for a filename string (includes the dot: .png)
	[[nodiscard]] const char* GetFileExtension(const char* fileName);
	// Get pointer to filename for a path string
	[[nodiscard]] std::string GetFileName(std::string_view filePath);

	// Convert a path to the format required by the operating system.
	[[nodiscard]] std::string NativePath(const std::string& pathName);
}