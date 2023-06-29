#include "stdafx.h"
#include "StringUtilities.h"
//-----------------------------------------------------------------------------
int string::Compare(const ea::string_view& a, const ea::string_view& b, bool caseSensitive)
{
	if( a.data() == b.data() ) return true;
	if( a.empty() ) return -1;
	if( b.empty() ) return 1;

	int cmp = 0;
	if( caseSensitive ) cmp = ea::Compare(a.data(), b.data(), ea::min(a.length(), b.length()));
	else cmp = ea::CompareI(a.data(), b.data(), ea::min(a.length(), b.length()));

	return (cmp != 0 ? cmp : (a.length() < b.length() ? -1 : (a.length() > b.length() ? 1 : 0)));
}
//-----------------------------------------------------------------------------