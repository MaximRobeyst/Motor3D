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

std::string Logger::SimplifyPath(const std::string& str, size_t dirSize) const
{
	std::string s = str;
	std::string drive;
	if (s.starts_with("local://")) {
		drive = "local://";
		s = s.substr(8, s.length());
	}
	else if (s.starts_with("res://")) {
		drive = "res://";
		s = s.substr(6, s.length());
	}
	else if (s.starts_with("user://")) {
		drive = "user://";
		s = s.substr(7, s.length());
	}
	else if (s.starts_with("//") || s.starts_with("\\\\")) {
		drive = s.substr(0, 2);
		s = s.substr(2, s.length() - 2);
	}
	else if (s.starts_with("/") || s.starts_with("\\")) {
		drive = s.substr(0, 1);
		s = s.substr(1, s.length() - 1);
	}
	else {
		size_t p = s.find(":/");
		if (p == std::string::npos) {
			p = s.find(":\\");
		}
		if (p != std::string::npos && p < s.find("/")) {
			drive = s.substr(0, p + 2);
			s = s.substr(p + 2, s.length());
		}
	}

	s = Replace(s, "\\", "/");
	while (true) { //In case of using 2 or more slash
		std::string compare = Replace(s, "//", "/");
		if (s == compare) {
			break;
		}
		else {
			s = compare;
		}
	}
	std::vector<std::string> dirs = Split(s, "/", false);

	for (auto i = dirs.begin(); i != dirs.end();)
	{
		if (*i == ".")
			i = dirs.erase(i);
		else if (*i == "..")
		{
			if (i == dirs.begin())
			{
				i = dirs.erase(i);
			}
			else
			{
				i = dirs.erase(i - 1);
				i = dirs.erase(i);
			}
		}
		else
			++i;
	}

	s = "";

	if (dirSize == 0 || dirSize > dirs.size())
		dirSize = 0;
	else
	{
		dirSize = dirs.size() - dirSize;
		s += "...";
	}

	for (size_t i = dirSize; i < dirs.size(); i++) {
		if (i > 0) {
			s += "/";
		}
		s += dirs[i];
	}

	return drive + s;
}

Logger::Logger()
{
	m_pLogWindow = new LogWindow();

	MyEngine::GetSingleton()->GetApplication()->AddWindowEditor(m_pLogWindow);
}

std::vector<std::string> Logger::Split(const std::string& str, const std::string& delim, const bool trim_empty) const
{
	size_t pos, last_pos = 0, len;
	std::vector<std::string> tokens;

	while (true) {
		pos = str.find(delim, last_pos);
		if (pos == std::string::npos) {
			pos = str.size();
		}

		len = pos - last_pos;
		if (!trim_empty || len != 0) {
			tokens.emplace_back(str.substr(last_pos, len));
		}

		if (pos == str.size()) {
			break;
		}
		else {
			last_pos = pos + delim.size();
		}
	}

	return tokens;
}

std::string Logger::Replace(const std::string& source, const char* key, const char* with) const
{
	std::string new_string;
	size_t search_from = 0;
	size_t result;

	while ((result = source.find(key, search_from)) != std::string::npos) {
		new_string += source.substr(search_from, result - search_from);
		new_string += with;
		size_t k = 0;
		while (key[k] != '\0') {
			k++;
		}
		search_from = result + k;
	}

	if (search_from == 0) {
		return source;
	}

	new_string += source.substr(search_from, source.length() - search_from);

	return new_string;
}

void Logger::Initialize()
{
}

void Logger::Cleanup()
{
	delete m_pInstance;
	m_pInstance = nullptr;
}

void Logger::Log(LogLevel level, const std::string& msg, const std::source_location location) const
{
#ifdef NDEBUG
	if (level == LogLevel::Debug || level == LogLevel::ToDo)
	{
		return;
	}
#endif

	std::stringstream stream;

	if (m_AppendTimestamp)
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		stream << "[" << st.wDay << "-" << st.wMonth << "-" << st.wYear << " - ";
		stream << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << "]";
	}
	stream << "[" << SimplifyPath(location.file_name(), 2) << "(" << std::to_string(location.line()) << ")" << "] ";
	//stream << "[" << SimplifyPath(__FILE__, 2) << "(" << std::to_string(__LINE__) << ")" << "] ";

	switch (level)
	{
	case LogLevel::Debug:
		stream << ("([DEBUG])");
		break;
	case LogLevel::Info:
		stream << ("[INFO]");
		break;
	case LogLevel::Warning:
		stream << ("[WARNING]");
		break;
	case LogLevel::Error:
		stream << ("[ERROR]");
		break;
	case LogLevel::ToDo:
		stream << ("[FIX-ME]");
		break;
	default:;
	}

	stream << ": " << msg << '\n';

	if (m_os)
	{
		(*m_os) << stream.str();
		m_os->flush();
	}

	//if error, break
	if (level == LogLevel::Error)
	{
		MessageBoxA(0, msg.c_str(), "ERROR", MB_OK | MB_ICONERROR);
	}

	m_pLogWindow->AddLog(stream.str().c_str());
}

void Logger::LogDebug(const std::string& msg, const std::source_location location) const
{
	Log(LogLevel::Debug, msg, location);
}

void Logger::LogInfo(const std::string& msg, const std::source_location location) const
{
	Log(LogLevel::Debug, msg, location);
}

void Logger::LogWarning(const std::string& msg, const std::source_location location) const
{
	Log(LogLevel::Warning, msg, location);
}

void Logger::LogErrorAndBreak(const std::string& msg, const std::source_location location) const
{
	Log(LogLevel::Error, msg, location);
}

void Logger::LogToDo(const std::string& source, const std::source_location location) const
{
	Log(LogLevel::ToDo, source, location);
}

