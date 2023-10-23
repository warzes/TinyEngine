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
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::vec2& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 2)
		return false;

	char* ptr = const_cast<char*>(string);
	out.x = (float)strtod(ptr, &ptr);
	out.y = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::vec3& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 3)
		return false;

	char* ptr = const_cast<char*>(string);
	out.x = (float)strtod(ptr, &ptr);
	out.y = (float)strtod(ptr, &ptr);
	out.z = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::vec4& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 4)
		return false;

	char* ptr = const_cast<char*>(string);
	out.x = (float)strtod(ptr, &ptr);
	out.y = (float)strtod(ptr, &ptr);
	out.z = (float)strtod(ptr, &ptr);
	out.w = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::ivec2& out, const char* string)
{
	const size_t elements = CountElements(string);
	if (elements < 2)
		return false;

	char* ptr = const_cast<char*>(string);
	out.x = strtol(ptr, &ptr, 10);
	out.y = strtol(ptr, &ptr, 10);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::ivec3& out, const char* string)
{
	size_t elements = CountElements(string, ' ');
	if (elements < 3)
		return false;

	char* ptr = const_cast<char*>(string);
	out.x = strtol(ptr, &ptr, 10);
	out.y = strtol(ptr, &ptr, 10);
	out.z = strtol(ptr, &ptr, 10);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::quat& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 3)
		return false;

	char* ptr = const_cast<char*>(string);
	if (elements >= 4)
	{
		out.w = (float)strtod(ptr, &ptr);
		out.x = (float)strtod(ptr, &ptr);
		out.y = (float)strtod(ptr, &ptr);
		out.z = (float)strtod(ptr, &ptr);
	}
	else
	{
		float x, y, z;
		x = (float)strtod(ptr, &ptr);
		y = (float)strtod(ptr, &ptr);
		z = (float)strtod(ptr, &ptr);
		out = glm::quat(glm::vec3(x, y, z));
	}

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::mat3& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 9)
		return false;

	char* ptr = const_cast<char*>(string);
	out[0][0] = (float)strtod(ptr, &ptr);
	out[0][1] = (float)strtod(ptr, &ptr);
	out[0][2] = (float)strtod(ptr, &ptr);
	out[1][0] = (float)strtod(ptr, &ptr);
	out[1][1] = (float)strtod(ptr, &ptr);
	out[1][2] = (float)strtod(ptr, &ptr);
	out[2][0] = (float)strtod(ptr, &ptr);
	out[2][1] = (float)strtod(ptr, &ptr);
	out[2][2] = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::mat3x4& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 12)
		return false;

	char* ptr = const_cast<char*>(string);
	out[0][0] = (float)strtod(ptr, &ptr);
	out[0][1] = (float)strtod(ptr, &ptr);
	out[0][2] = (float)strtod(ptr, &ptr);
	out[0][3] = (float)strtod(ptr, &ptr);
	out[1][0] = (float)strtod(ptr, &ptr);
	out[1][1] = (float)strtod(ptr, &ptr);
	out[1][2] = (float)strtod(ptr, &ptr);
	out[1][3] = (float)strtod(ptr, &ptr);
	out[2][0] = (float)strtod(ptr, &ptr);
	out[2][1] = (float)strtod(ptr, &ptr);
	out[2][2] = (float)strtod(ptr, &ptr);
	out[2][3] = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(glm::mat4& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 16)
		return false;

	char* ptr = const_cast<char*>(string);
	out[0][0] = (float)strtod(ptr, &ptr);
	out[0][1] = (float)strtod(ptr, &ptr);
	out[0][2] = (float)strtod(ptr, &ptr);
	out[0][3] = (float)strtod(ptr, &ptr);
	out[1][0] = (float)strtod(ptr, &ptr);
	out[1][1] = (float)strtod(ptr, &ptr);
	out[1][2] = (float)strtod(ptr, &ptr);
	out[1][3] = (float)strtod(ptr, &ptr);
	out[2][0] = (float)strtod(ptr, &ptr);
	out[2][1] = (float)strtod(ptr, &ptr);
	out[2][2] = (float)strtod(ptr, &ptr);
	out[2][3] = (float)strtod(ptr, &ptr);
	out[3][0] = (float)strtod(ptr, &ptr);
	out[3][1] = (float)strtod(ptr, &ptr);
	out[3][2] = (float)strtod(ptr, &ptr);
	out[3][3] = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::vec2& in)
{
	return std::string(std::to_string(in.x) + " " + std::to_string(in.y));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::vec3& in)
{
	return std::string(std::to_string(in.x) + " " + std::to_string(in.y) + " " + std::to_string(in.z));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::vec4& in)
{
	return std::string(std::to_string(in.x) + " " + std::to_string(in.y) + " " + std::to_string(in.z) + " " + std::to_string(in.w));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::ivec2& in)
{
	return std::string(std::to_string(in.x) + " " + std::to_string(in.y));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::ivec3& in)
{
	return std::string(std::to_string(in.x) + " " + std::to_string(in.y) + " " + std::to_string(in.z));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::quat& in)
{
	return std::string(std::to_string(in.w) + " " + std::to_string(in.x) + " " + std::to_string(in.y) + " " + std::to_string(in.z));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::mat3& in)
{
	return std::string(
		std::to_string(in[0][0]) + " " + std::to_string(in[0][1]) + " " + std::to_string(in[0][2]) + " " +
		std::to_string(in[1][0]) + " " + std::to_string(in[1][1]) + " " + std::to_string(in[1][2]) + " " +
		std::to_string(in[2][0]) + " " + std::to_string(in[2][1]) + " " + std::to_string(in[2][2]));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::mat3x4& in)
{
	return std::string(
		std::to_string(in[0][0]) + " " + std::to_string(in[0][1]) + " " + std::to_string(in[0][2]) + " " + std::to_string(in[0][3]) + " " +
		std::to_string(in[1][0]) + " " + std::to_string(in[1][1]) + " " + std::to_string(in[1][2]) + " " + std::to_string(in[1][3]) + " " +
		std::to_string(in[2][0]) + " " + std::to_string(in[2][1]) + " " + std::to_string(in[2][2]) + " " + std::to_string(in[2][3]));
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const glm::mat4& in)
{
	return std::string(
		std::to_string(in[0][0]) + " " + std::to_string(in[0][1]) + " " + std::to_string(in[0][2]) + " " + std::to_string(in[0][3]) + " " +
		std::to_string(in[1][0]) + " " + std::to_string(in[1][1]) + " " + std::to_string(in[1][2]) + " " + std::to_string(in[1][3]) + " " +
		std::to_string(in[2][0]) + " " + std::to_string(in[2][1]) + " " + std::to_string(in[2][2]) + " " + std::to_string(in[2][3]) + " " + 
		std::to_string(in[3][0]) + " " + std::to_string(in[3][1]) + " " + std::to_string(in[3][2]) + " " + std::to_string(in[3][3]));
}
//-----------------------------------------------------------------------------