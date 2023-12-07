#include "stdafx.h"
#include "Color.h"
#include "Core/Utilities/StringUtilities.h"
//-----------------------------------------------------------------------------
const Color Color::Black = Color{ 0, 0, 0, 255 };
const Color Color::Red = Color{ 255, 0, 0, 255 };
const Color Color::Magenta = Color{ 255, 0, 255, 255 };
const Color Color::Green = Color{ 0, 255, 0, 255 };
const Color Color::Cyan = Color{ 0, 255, 255, 255 };
const Color Color::Blue = Color{ 0, 0, 255, 255 };
const Color Color::Yellow = Color{ 255, 255, 0, 255 };
const Color Color::White = Color{ 255, 255, 255, 255 };
const Color Color::Gray = Color{ 128, 128, 128, 255 };
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