#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
LogSystem::~LogSystem()
{
	destroy();
}
//-----------------------------------------------------------------------------
void LogSystem::Print(const std::string& msg)
{
	puts(msg.c_str());
#if defined(_WIN32)
	if (m_logFile) fputs((msg + "\n").c_str(), m_logFile);
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::Warning(const std::string& msg)
{
	Print("Warning: " + msg);
}
//-----------------------------------------------------------------------------
void LogSystem::Error(const std::string& msg)
{
	Print("Error: " + msg);
}
//-----------------------------------------------------------------------------
void LogSystem::Fatal(const std::string& msg)
{
	HelperSubsystem::ExitRequest();
	Error(msg);
}
//-----------------------------------------------------------------------------
void LogSystem::create(const LogCreateInfo& createInfo)
{
#if defined(_WIN32)
	errno_t fileErr = fopen_s(&m_logFile, createInfo.fileName.c_str(), "w");
	if (fileErr != 0 || !m_logFile)
	{
		Error("LogCreate() failed!!!");
		m_logFile = nullptr;
	}
#endif

	HelperSubsystem::initHelper();
}
//-----------------------------------------------------------------------------
void LogSystem::destroy()
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