#include "stdafx.h"
#include "IntBox.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
bool StringUtils::FromString(IntBox& out, const char* string)
{
	size_t elements = CountElements(string);
	if (elements < 6)
		return false;

	char* ptr = const_cast<char*>(string);
	out.left = strtol(ptr, &ptr, 10);
	out.top = strtol(ptr, &ptr, 10);
	out.near = strtol(ptr, &ptr, 10);
	out.right = strtol(ptr, &ptr, 10);
	out.bottom = strtol(ptr, &ptr, 10);
	out.far = strtol(ptr, &ptr, 10);

	return true;
}
//-----------------------------------------------------------------------------
std::string StringUtils::ToString(const IntBox& in)
{
	return std::string(std::to_string(in.left) + " " + std::to_string(in.top) + " " + std::to_string(in.near) + " " + std::to_string(in.right) + " " + std::to_string(in.bottom) + " " + std::to_string(in.far));
}
//-----------------------------------------------------------------------------