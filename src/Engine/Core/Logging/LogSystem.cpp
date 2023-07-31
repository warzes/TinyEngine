#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
LogSystem gLogSystem;
//-----------------------------------------------------------------------------
LogSystem::~LogSystem()
{
	Destroy();
}
//-----------------------------------------------------------------------------
bool LogSystem::Create(const LogCreateInfo& createInfo) noexcept
{
#if defined(_WIN32)
	errno_t fileErr = fopen_s(&m_logFile, createInfo.fileName.c_str(), "w");
	if (fileErr != 0 || !m_logFile)
	{
		Error("LogCreate() failed!!!");
		m_logFile = nullptr;
	}
#endif
	return true;
}
//-----------------------------------------------------------------------------
void LogSystem::Destroy()
{
#if defined(_WIN32)
	if (m_logFile)
	{
		fclose(m_logFile);
		m_logFile = nullptr;
	}
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::Print(const std::string& msg) noexcept
{
	puts(msg.c_str());
#if defined(_WIN32)
	if (m_logFile) fputs((msg + "\n").c_str(), m_logFile);
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::Warning(const std::string& msg) noexcept
{
	Print("Warning: " + msg);
}
//-----------------------------------------------------------------------------
void LogSystem::Error(const std::string& msg) noexcept
{
	Print("Error: " + msg);
}
//-----------------------------------------------------------------------------
void LogSystem::Fatal(const std::string& msg) noexcept
{
	extern void ExitRequest();
	ExitRequest();
	Error(msg);
}
//-----------------------------------------------------------------------------
LogSystem& GetLogSystem()
{
	return gLogSystem;
}
//-----------------------------------------------------------------------------