#include "stdafx.h"
#include "Rect.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
bool StringUtils::FromString(Rect& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 4)
		return false;

	char* ptr = const_cast<char*>(string);
	out.min.x = (float)strtod(ptr, &ptr);
	out.min.y = (float)strtod(ptr, &ptr);
	out.max.x = (float)strtod(ptr, &ptr);
	out.max.y = (float)strtod(ptr, &ptr);

	return true;
}
//-----------------------------------------------------------------------------
bool StringUtils::FromString(IntRect& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 4)
		return false;

	char* ptr = const_cast<char*>(string);
	out.left = strtol(ptr, &ptr, 10);
	out.top = strtol(ptr, &ptr, 10);
	out.right = strtol(ptr, &ptr, 10);
	out.bottom = strtol(ptr, &ptr, 10);

	return true;
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const Rect& in)
{
	return ToString(in.min) + " " + ToString(in.max);
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const IntRect& in)
{
	return std::string(std::to_string(in.left) + " " + std::to_string(in.top) + " " + std::to_string(in.right) + " " + std::to_string(in.bottom));
}
//-----------------------------------------------------------------------------