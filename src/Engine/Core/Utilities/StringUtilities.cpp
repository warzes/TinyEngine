#include "stdafx.h"
#include "StringUtilities.h"
//-----------------------------------------------------------------------------
size_t StringUtils::CountElements(const std::string& string, char separator)
{
	return CountElements(string.c_str(), separator);
}
//-----------------------------------------------------------------------------
size_t StringUtils::CountElements(const char* string, char separator)
{
	if (!string)
		return 0;

	const char* endPos = string + strlen(string);
	const char* pos = string;
	size_t ret = 0;

	while (pos < endPos)
	{
		if (*pos != separator)
			break;
		++pos;
	}

	while (pos < endPos)
	{
		const char* start = pos;

		while (start < endPos)
		{
			if (*start == separator)
				break;

			++start;
		}

		if (start == endPos)
		{
			++ret;
			break;
		}

		const char* end = start;

		while (end < endPos)
		{
			if (*end != separator)
				break;

			++end;
		}

		++ret;
		pos = end;
	}

	return ret;
}
//-----------------------------------------------------------------------------
std::string StringUtils::Trim(const std::string& string)
{
	size_t trimStart = 0;
	size_t trimEnd = string.length();

	while (trimStart < trimEnd)
	{
		char c = string[trimStart];
		if (c != ' ' && c != 9)
			break;
		++trimStart;
	}
	while (trimEnd > trimStart)
	{
		char c = string[trimEnd - 1];
		if (c != ' ' && c != 9)
			break;
		--trimEnd;
	}

	return string.substr(trimStart, trimEnd - trimStart);
}
//-----------------------------------------------------------------------------
std::string StringUtils::Replace(const std::string& string, const std::string& replaceThis, const std::string& replaceWith)
{
	std::string ret(string);
	size_t idx = ret.find(replaceThis);

	while (idx != std::string::npos)
	{
		ret.replace(idx, replaceThis.length(), replaceWith);
		idx += replaceWith.length();
		idx = ret.find(replaceThis, idx);
	}

	return ret;
}
//-----------------------------------------------------------------------------
std::string StringUtils::Replace(const std::string& string, char replaceThis, char replaceWith)
{
	std::string ret(string);

	for (size_t i = 0; i < ret.length(); ++i)
	{
		if (ret[i] == replaceThis)
			ret[i] = replaceWith;
	}

	return ret;
}
//-----------------------------------------------------------------------------
void StringUtils::ReplaceInPlace(std::string& string, const std::string& replaceThis, const std::string& replaceWith)
{
	size_t idx = string.find(replaceThis);

	while (idx != std::string::npos)
	{
		string.replace(idx, replaceThis.length(), replaceWith);
		idx += replaceWith.length();
		idx = string.find(replaceThis, idx);
	}
}
//-----------------------------------------------------------------------------
void StringUtils::ReplaceInPlace(std::string& string, char replaceThis, char replaceWith)
{
	for (size_t i = 0; i < string.length(); ++i)
	{
		if (string[i] == replaceThis)
			string[i] = replaceWith;
	}
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToUpper(const std::string& string)
{
	std::string ret(string);

	for (size_t i = 0; i < ret.length(); ++i)
		ret[i] = (char)toupper(ret[i]);

	return ret;
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToLower(const std::string& string)
{
	std::string ret(string);

	for (size_t i = 0; i < ret.length(); ++i)
		ret[i] = (char)tolower(ret[i]);

	return ret;
}
//-----------------------------------------------------------------------------
bool StringUtils::StartsWith(const std::string& string, const std::string& substring)
{
	return string.find(substring) == 0;
}
//-----------------------------------------------------------------------------
bool StringUtils::EndsWith(const std::string& string, const std::string& substring)
{
	return string.rfind(substring) == string.length() - substring.length();
}
//-----------------------------------------------------------------------------
std::vector<std::string> StringUtils::Split(const std::string& string, char separator)
{
	return Split(string.c_str(), separator);
}
//-----------------------------------------------------------------------------
std::vector<std::string> StringUtils::Split(const char* string, char separator)
{
	std::vector<std::string> ret;
	if (!string)
		return ret;

	size_t pos = 0;
	size_t length = strlen(string);

	while (pos < length)
	{
		if (string[pos] != separator)
			break;
		++pos;
	}

	while (pos < length)
	{
		size_t start = pos;

		while (start < length)
		{
			if (string[start] == separator)
				break;

			++start;
		}

		if (start == length)
		{
			ret.push_back(std::string(&string[pos]));
			break;
		}

		size_t end = start;

		while (end < length)
		{
			if (string[end] != separator)
				break;

			++end;
		}

		ret.push_back(std::string(&string[pos], start - pos));
		pos = end;
	}

	return ret;
}
//-----------------------------------------------------------------------------
size_t StringUtils::ListIndex(const std::string& string, const std::string* strings, size_t defaultIndex)
{
	return ListIndex(string.c_str(), strings, defaultIndex);
}
//-----------------------------------------------------------------------------
size_t StringUtils::ListIndex(const char* string, const std::string* strings, size_t defaultIndex)
{
	size_t i = 0;

	while (!strings[i].empty())
	{
		if (strings[i] == string)
			return i;
		++i;
	}

	return defaultIndex;
}
//-----------------------------------------------------------------------------
size_t StringUtils::ListIndex(const std::string& string, const char** strings, size_t defaultIndex)
{
	return ListIndex(string.c_str(), strings, defaultIndex);
}
//-----------------------------------------------------------------------------
size_t StringUtils::ListIndex(const char* string, const char** strings, size_t defaultIndex)
{
	size_t i = 0;

	while (strings[i])
	{
		if (!strcmp(string, strings[i]))
			return i;
		++i;
	}

	return defaultIndex;
}
bool StringUtils::FromString(glm::vec2& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::vec3& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::vec4& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::ivec2& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::ivec3& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::mat3& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::mat3x4& out, const char* string)
{
	return false;
}
bool StringUtils::FromString(glm::mat4& out, const char* string)
{
	return false;
}
std::string StringUtils::ToString(const glm::vec2& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::vec3& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::vec4& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::ivec2& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::ivec3& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::mat3& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::mat3x4& in)
{
	return std::string();
}
std::string StringUtils::ToString(const glm::mat4& in)
{
	return std::string();
}
//-----------------------------------------------------------------------------