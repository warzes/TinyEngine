#include "stdafx.h"
#include "Error.h"
//-----------------------------------------------------------------------------
bool IsError = false;
bool IsFatal = false;
std::string ErrorText = "";
//-----------------------------------------------------------------------------
void Error(const std::string& error)
{
	ErrorText = error;
	IsError = true;
}
//-----------------------------------------------------------------------------
void Fatal(const std::string& error)
{
	ErrorText = error;
	IsFatal = true;
}
//-----------------------------------------------------------------------------