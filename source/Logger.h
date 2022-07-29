#pragma once
#include <chrono>
#include <source_location>


// From the overlord engine

enum class LogLevel
{
	Debug = 0x1,
	Info = 0x2,
	Warning = 0x4,
	Error = 0x8,
	Todo = 0x16
};

class LogWindow;

class Logger final 
{
public:
	virtual ~Logger() = default;
	Logger(const Logger & other) = delete;
	Logger(Logger && other) noexcept = delete;
	Logger& operator=(const Logger & other) = delete;
	Logger& operator=(Logger && other) noexcept = delete;

	static Logger* GetInstance();

	void Initialize();
	void Cleanup();

	void Log(LogLevel level, const std::string& msg, const std::source_location location = std::source_location::current()) const;
	void LogDebug(const std::string& msg, const std::source_location location = std::source_location::current()) const;
	void LogInfo(const std::string& msg, const std::source_location location = std::source_location::current()) const;
	void LogWarning(const std::string& msg, const std::source_location location = std::source_location::current()) const;
	void LogErrorAndBreak(const std::string& msg, const std::source_location location = std::source_location::current()) const;
	void LogToDo(const std::string& source, const std::source_location location = std::source_location::current()) const;

	static void ClearConsole();

	void BreakOnLog(LogLevel level, bool doBreak) { doBreak ? m_BreakBitField |= static_cast<int>(level) : m_BreakBitField &= ~static_cast<int>(level); }

	void AppendTimestamp(bool append) { m_AppendTimestamp = append; }

	static std::string SimplifyPath(const std::string& str, size_t dirSize = 0);


private:
	Logger();

	static Logger* m_pInstance;

	LogWindow* m_pLogWindow{};

	std::ostream* m_os{};
	std::ofstream* m_FileOutStream{};

	double m_PcFreq{};
	static constexpr int MAX_PERFORMANCE_TIMERS{ 10 };
	__int64 m_PerformanceTimerArr[MAX_PERFORMANCE_TIMERS]{};
	std::chrono::high_resolution_clock::time_point m_TimePoints[MAX_PERFORMANCE_TIMERS]{};

	int m_BreakBitField{ static_cast<int>(LogLevel::Error) };
	bool m_AppendTimestamp{ false };

	void* m_ConsoleHandle{ nullptr };
};
