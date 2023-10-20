#include "stdafx.h"
#include "StringHash.h"
//-----------------------------------------------------------------------------
const StringHash StringHash::Zero;
//-----------------------------------------------------------------------------
std::string StringHash::ToString() const
{
	char formatBuffer[1024];
	sprintf_s(formatBuffer, 1024, "%08X", m_value);
	return std::string(formatBuffer);
}
//-----------------------------------------------------------------------------
unsigned StringHash::Calculate(const char* str)
{
	unsigned hash = 0;
	while (*str)
	{
		hash = (tolower(*str)) + (hash << 6) + (hash << 16) - hash;
		++str;
	}

	return hash;
}
//-----------------------------------------------------------------------------