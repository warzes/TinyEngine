#pragma once

namespace FileSystem
{
	// Returns true if the file exists, false otherwise.
	[[nodiscard]] bool Exists(const std::string& path) noexcept;
	// Returns true if the file is directory, false otherwise.
	[[nodiscard]] bool IsDirectory(const std::string& path) noexcept;


	bool CreateDir(const std::string& pathName);
	bool RenameFile(const std::string& srcFileName, const std::string& destFileName);
	bool DeleteFile(const std::string& fileName);
	
	// Return the file's last modified time as seconds since epoch, or 0 if can not be accessed.
	[[nodiscard]] unsigned LastModifiedTime(const std::string& fileName);
	// Set the file's last modified time as seconds since epoch. Return true on success.
	[[nodiscard]] bool SetLastModifiedTime(const std::string& fileName, unsigned newTime);





#if !PLATFORM_EMSCRIPTEN // TODO:
	[[nodiscard]] std::optional<std::vector<uint8_t>> FileToMemory(const std::string& fileName, unsigned int* bytesRead = nullptr);
#endif

	// TODO: убрать из кода заменив на std::filesystem

	
	// Get pointer to extension for a filename string (includes the dot: .png)
	[[nodiscard]] const char* GetFileExtension(const char* fileName);
	// Get pointer to filename for a path string
	[[nodiscard]] std::string GetFileName(std::string_view filePath);

	// Split a full path to path, filename and extension.
	void SplitPath(const std::string& fullPath, std::string& pathName, std::string& fileName, std::string& extension, bool lowercaseExtension = false);
	/// Return the path from a full path.
	std::string Path(const std::string& fullPath);
	/// Return the filename from a full path.
	std::string FileName(const std::string& fullPath);
	/// Return the path and filename from a full path.
	std::string PathAndFileName(const std::string& fullPath);
	/// Return the extension from a full path.
	std::string Extension(const std::string& fullPath, bool lowercaseExtension = false);
	// Return the filename and extension from a full path. The case of the extension is preserved by default, so that the file can be opened in case-sensitive operating systems.
	std::string FileNameAndExtension(const std::string& fullPath, bool lowercaseExtension = false);

	// Add a slash at the end of the path if missing and convert to normalized format (use slashes.)
	std::string AddTrailingSlash(const std::string& pathName);
	/// Remove the slash from the end of a path if exists and convert to normalized format (use slashes.)
	std::string RemoveTrailingSlash(const std::string& pathName);

	// Convert a path to normalized (internal) format which uses slashes.
	[[nodiscard]] std::string NormalizePath(const std::string& pathName);
	// Convert a path to the format required by the operating system.
	[[nodiscard]] std::string NativePath(const std::string& pathName);

	// Return whether a path is absolute.
	bool IsAbsolutePath(const std::string& pathName);

	bool IsFileExtension(const std::string& pathName, const std::string& extension);

}