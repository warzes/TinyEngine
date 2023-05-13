#pragma once

namespace FileSystem
{
	[[nodiscard]] std::optional<std::vector<uint8_t>> FileToMemory(const std::string& fileName, unsigned int* bytesRead = nullptr);

	// Get pointer to extension for a filename string (includes the dot: .png)
	[[nodiscard]] const char* GetFileExtension(const char* fileName);
	// Get pointer to filename for a path string
	[[nodiscard]] const char* GetFileName(const char* filePath);
}