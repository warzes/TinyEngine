#pragma once

#if _WIN32
using WideChar = wchar_t;
using WideString = ea::wstring;
#else
using WideChar = char16_t;
using WideString = ea::string16;
#endif

namespace string
{
	// Compare string a with string b, optionally ignoring case sensitivity.
	[[nodiscard]] int Compare(const ea::string_view& a, const ea::string_view& b, bool caseSensitive = true);

} // namespace string