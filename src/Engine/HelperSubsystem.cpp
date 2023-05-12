#include "stdafx.h"
#include "HelperSubsystem.h"
#include "SubsystemManager.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
std::shared_ptr<LogSystem> HelperSubsystem::GetLogSystem()
{
	return m_logSystem;
}
//-----------------------------------------------------------------------------
void HelperSubsystem::ExitRequest()
{
}
//-----------------------------------------------------------------------------
void HelperSubsystem::LogPrint(const std::string& msg)
{
	m_logSystem->Print(msg);
}
//-----------------------------------------------------------------------------
void HelperSubsystem::LogWarning(const std::string& msg)
{
	m_logSystem->Warning(msg);
}
//-----------------------------------------------------------------------------
void HelperSubsystem::LogError(const std::string& msg)
{
	m_logSystem->Error(msg);
}
//-----------------------------------------------------------------------------
void HelperSubsystem::LogFatal(const std::string& msg)
{
	m_logSystem->Fatal(msg);
}
//-----------------------------------------------------------------------------
void HelperSubsystem::initHelper()
{
	m_logSystem = GetSubsystem<LogSystem>();
	assert(m_logSystem);
}
//-----------------------------------------------------------------------------