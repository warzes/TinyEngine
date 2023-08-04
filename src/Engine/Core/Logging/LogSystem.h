#pragma once

struct LogCreateInfo final
{
	std::string fileName = "../log.txt";
};

class LogSystem final
{
public:
	LogSystem() = default;
	~LogSystem();

	bool Create(const LogCreateInfo& createInfo) noexcept;
	void Destroy();

	void Print(const std::string& msg) noexcept;
	void Warning(const std::string& msg) noexcept;
	void Error(const std::string& msg) noexcept;
	void Fatal(const std::string& msg) noexcept;

private:
	LogSystem(LogSystem&&) = delete;
	LogSystem(const LogSystem&) = delete;
	LogSystem& operator=(LogSystem&&) = delete;
	LogSystem& operator=(const LogSystem&) = delete;

#if PLATFORM_DESKTOP
	FILE* m_logFile = nullptr;
#endif
};

LogSystem& GetLogSystem();