#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
extern LogSystem gLogSystem;
//-----------------------------------------------------------------------------
void LogPrint(const std::string& msg)
{
	gLogSystem.Print(msg);
}
//-----------------------------------------------------------------------------
void LogWarning(const std::string& msg)
{
	gLogSystem.Warning(msg);
}
//-----------------------------------------------------------------------------
void LogError(const std::string& msg)
{
	gLogSystem.Error(msg);
}
//-----------------------------------------------------------------------------
void LogFatal(const std::string& msg)
{
	gLogSystem.Fatal(msg);
}
//-----------------------------------------------------------------------------