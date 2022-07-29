#include "Logger.h"
#include "LogWindow.h"
#include "MyEngine.h"
#include "MyApplication.h"

Logger* Logger::m_pInstance{};

Logger* Logger::GetInstance()
{
	if (m_pInstance == nullptr) m_pInstance = new Logger();

	return m_pInstance;
}

Logger::Logger()
{
	m_pLogWindow = new LogWindow();

	MyEngine::GetSingleton()->GetApplication()->AddWindowEditor(m_pLogWindow);
}

void Logger::Initialize()
{
}

void Logger::Cleanup()
{
	delete m_pInstance;
	m_pInstance = nullptr;
}

void Logger::Log(LogLevel /*level*/, const std::string& msg, const std::source_location /*location*/) const
{
	m_pLogWindow->AddLog(msg.c_str());
}

