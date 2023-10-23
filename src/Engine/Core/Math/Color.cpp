#include "stdafx.h"
#include "Color.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
bool StringUtils::FromString(ColorF& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 3)
		return false;
	char* ptr = const_cast<char*>(string);
	out.r = (float)strtod(ptr, &ptr);
	out.g = (float)strtod(ptr, &ptr);
	out.b = (float)strtod(ptr, &ptr);
	if (elements > 3)
		out.a = (float)strtod(ptr, &ptr);
	else
		out.a = 1.0f;

	return true;
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const ColorF& in)
{
	return std::string(std::to_string(in.r) + " " + std::to_string(in.g) + " " + std::to_string(in.b) + " " + std::to_string(in.a));
}
//-----------------------------------------------------------------------------