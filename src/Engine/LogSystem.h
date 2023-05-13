#pragma once

struct LogCreateInfo
{
	std::string fileName = "../log.txt";
};

class LogSystem final
{
	friend class EngineDevice;
public:
	LogSystem() = default;
	~LogSystem();

	void Create(const LogCreateInfo& createInfo);
	void Destroy();

	void Print(const std::string& msg);
	void Warning(const std::string& msg);
	void Error(const std::string& msg);
	void Fatal(const std::string& msg);

private:
	LogSystem(LogSystem&&) = delete;
	LogSystem(const LogSystem&) = delete;
	LogSystem& operator=(LogSystem&&) = delete;
	LogSystem& operator=(const LogSystem&) = delete;

	FILE* m_logFile = nullptr;
};

LogSystem& GetLogSystem();