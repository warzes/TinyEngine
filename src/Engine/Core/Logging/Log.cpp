#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
extern LogSystem gLogSystem;
//-----------------------------------------------------------------------------
void LogPrint(const std::string& msg) noexcept
{
	gLogSystem.Print(msg);
}
//-----------------------------------------------------------------------------
void LogWarning(const std::string& msg) noexcept
{
	gLogSystem.Warning(msg);
}
//-----------------------------------------------------------------------------
void LogError(const std::string& msg) noexcept
{
	gLogSystem.Error(msg);
}
//-----------------------------------------------------------------------------
void LogFatal(const std::string& msg) noexcept
{
	gLogSystem.Fatal(msg);
}
//-----------------------------------------------------------------------------