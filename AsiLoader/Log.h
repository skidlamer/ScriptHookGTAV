#pragma once

enum class eLogType : int {
	LogTypePrint,
	LogTypeDebug,
	LogTypeWarning,
	LogTypeError,
};

typedef std::map<eLogType, std::string> intStringMap;

class Log {
public:

	Log(std::ostream& ifile);
	~Log();

	void				Write(eLogType logType, const std::string& data);

	void				Message(const std::string& title, std::string& message);

	template <typename... Args> bool Stream(std::ostream& out, Args&&... args) {
		return print(out, std::forward<Args>(args)...);
	}

private:

	std::ostream& file;

	inline bool print(std::ostream& out) {
		return !!(out << std::endl);
	}

	template<typename T>
	bool print(std::ostream& out, T&& value)
	{
		return !!(out << std::forward<T>(value) << std::endl);
	}

	template<typename First, typename ... Rest>
	bool print(std::ostream& out, First&& first, Rest&& ... rest)
	{
		return !!(out << std::forward<First>(first)) && print(out, std::forward<Rest>(rest)...);
	}

	const std::string	GetTimeFormatted() const;

	intStringMap		logTypeToFormatMap;
	bool				firstEntry = true;
};  inline uniquePtr(Log) g_log;

template <typename... Args> void LOG_PRINT(Args&&... args) { g_log->Write(eLogType::LogTypePrint, fmt(args...)); }
template <typename... Args> void LOG_DEBUG(Args&&... args) { g_log->Write(eLogType::LogTypeDebug, fmt(args...)); }
template <typename... Args> void LOG_WARNING(Args&&... args) { g_log->Write(eLogType::LogTypeWarning, fmt(args...)); }
template <typename... Args> void LOG_ERROR(Args&&... args) { g_log->Write(eLogType::LogTypeError, fmt(args...)); }
template <typename... Args> void LOG_STREAM(Args&&... args) { std::stringstream ss; if (g_log->Stream(ss, args...)) { g_log->Write(eLogType::LogTypePrint, ss.str()); } }
template <typename... Args> void LOG_MESSAGE(const std::string& title, Args&&... args) { g_log->Message(title, fmt(args...)); }

