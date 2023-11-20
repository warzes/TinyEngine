#include "stdafx.h"
#include "FileSystem.h"
#include "Core/Logging/Log.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
bool FileSystem::Exists(const std::string& path) noexcept
{
	assert(!path.empty());
	return std::filesystem::exists(path);
}
//-----------------------------------------------------------------------------
bool FileSystem::IsDirectory(const std::string& path) noexcept
{
	assert(!path.empty());
	return std::filesystem::is_directory(path);
}
//-----------------------------------------------------------------------------
unsigned FileSystem::LastModifiedTime(const std::string& fileName)
{
	std::filesystem::file_time_type info = std::filesystem::last_write_time(fileName);
	return info.time_since_epoch().count();
}
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN // TODO:
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
#endif
//-----------------------------------------------------------------------------
const char* FileSystem::GetFileExtension(const char* fileName)
{
	const char* dot = strrchr(fileName, '.');
	if (!dot || dot == fileName) return nullptr;
	return dot;
}
//-----------------------------------------------------------------------------
std::string FileSystem::GetFileName(std::string_view filePath)
{
	std::size_t fileNamePos = filePath.find_last_of("/\\");
	if (fileNamePos == std::string::npos)
		return std::string(filePath);
	return std::string(filePath.substr(fileNamePos + 1));
}
//-----------------------------------------------------------------------------
void FileSystem::SplitPath(const std::string& fullPath, std::string& pathName, std::string& fileName, std::string& extension, bool lowercaseExtension)
{
	std::string fullPathCopy = NormalizePath(fullPath);

	size_t extPos = fullPathCopy.find_last_of('.');
	size_t pathPos = fullPathCopy.find_last_of('/');

	if (extPos != std::string::npos && (pathPos == std::string::npos || extPos > pathPos))
	{
		extension = fullPathCopy.substr(extPos);
		if (lowercaseExtension)
			extension = StringUtils::ToLower(extension);
		fullPathCopy = fullPathCopy.substr(0, extPos);
	}
	else
		extension.clear();

	pathPos = fullPathCopy.find_last_of('/');
	if (pathPos != std::string::npos)
	{
		fileName = fullPathCopy.substr(pathPos + 1);
		pathName = fullPathCopy.substr(0, pathPos + 1);
	}
	else
	{
		fileName = fullPathCopy;
		pathName.clear();
	}
}
//-----------------------------------------------------------------------------
std::string FileSystem::Path(const std::string& fullPath)
{
	std::string path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return path;
}
//-----------------------------------------------------------------------------
std::string FileSystem::FileName(const std::string& fullPath)
{
	std::string path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return file;
}
//-----------------------------------------------------------------------------
std::string FileSystem::PathAndFileName(const std::string& fullPath)
{
	std::string path, file, extension;
	SplitPath(fullPath, path, file, extension);
	return path + file;
}
//-----------------------------------------------------------------------------
std::string FileSystem::Extension(const std::string& fullPath, bool lowercaseExtension)
{
	std::string path, file, extension;
	SplitPath(fullPath, path, file, extension, lowercaseExtension);
	return extension;
}
//-----------------------------------------------------------------------------
std::string FileSystem::FileNameAndExtension(const std::string& fullPath, bool lowercaseExtension)
{
	std::string path, file, extension;
	SplitPath(fullPath, path, file, extension, lowercaseExtension);
	return file + extension;
}
//-----------------------------------------------------------------------------
std::string FileSystem::AddTrailingSlash(const std::string& pathName)
{
	std::string ret = StringUtils::Trim(pathName);
	StringUtils::ReplaceInPlace(ret, '\\', '/');
	if (!ret.empty() && ret.back() != '/')
		ret += '/';
	return ret;
}
//-----------------------------------------------------------------------------
std::string FileSystem::RemoveTrailingSlash(const std::string& pathName)
{
	std::string ret = StringUtils::Trim(pathName);
	StringUtils::ReplaceInPlace(ret, '\\', '/');
	if (!ret.empty() && ret.back() == '/')
		ret.resize(ret.length() - 1);
	return ret;
}
//-----------------------------------------------------------------------------
std::string FileSystem::NormalizePath(const std::string& pathName)
{
	std::string ret(pathName);
	StringUtils::ReplaceInPlace(ret, '\\', '/');
	return ret;
}
//-----------------------------------------------------------------------------
std::string FileSystem::NativePath(const std::string& pathName)
{
	std::string ret(pathName);
#if defined(_WIN32)
	StringUtils::ReplaceInPlace(ret, '/', '\\');
#endif
	return ret;
}
//-----------------------------------------------------------------------------
bool FileSystem::IsAbsolutePath(const std::string& pathName)
{
	if (pathName.empty())
		return false;

	std::string path = NormalizePath(pathName);

	if (path[0] == '/')
		return true;

#ifdef _WIN32
	if (path.length() > 1 && isalpha(path[0]) && path[1] == ':')
		return true;
#endif

	return false;
}
//-----------------------------------------------------------------------------
bool FileSystem::IsFileExtension(const std::string& pathName, const std::string& extension)
{
	// TODO: Extension() тут не очень эффективен - переделать
	return Extension(pathName, true) == extension;
}
//-----------------------------------------------------------------------------