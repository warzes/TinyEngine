#pragma once

#include "ISubsystem.h"

class LogSystem;

class HelperSubsystem : public ISubsystem
{
public:
	std::shared_ptr<LogSystem> GetLogSystem();

	void ExitRequest();

	void LogPrint(const std::string& msg);
	void LogWarning(const std::string& msg);
	void LogError(const std::string& msg);
	void LogFatal(const std::string& msg);

protected:
	HelperSubsystem() = default;
	HelperSubsystem(HelperSubsystem&&) = delete;
	HelperSubsystem(const HelperSubsystem&) = delete;
	HelperSubsystem& operator=(HelperSubsystem&&) = delete;
	HelperSubsystem& operator=(const HelperSubsystem&) = delete;

	void initHelper();

	std::shared_ptr<LogSystem> m_logSystem;
};