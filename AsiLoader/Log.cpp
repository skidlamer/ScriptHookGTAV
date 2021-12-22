#include "pch.h"
#include <time.h>

Log::Log(std::ostream& ifile)
	: file(ifile)
{
	logTypeToFormatMap[eLogType::LogTypePrint] = "";
	logTypeToFormatMap[eLogType::LogTypeDebug] = " [ Debug ]";
	logTypeToFormatMap[eLogType::LogTypeWarning] = " [ Warning ]";
	logTypeToFormatMap[eLogType::LogTypeError] = " [ Error ]";
}

Log::~Log() {
	g_log.release();
}

void Log::Write(eLogType logType, const std::string& data) {

	bool isDebug = logType == eLogType::LogTypeDebug;

	std::stringstream ss;

	ss << GetTimeFormatted() << logTypeToFormatMap[logType].c_str() << ' ' << data << std::endl;

	std::string st = ss.str();

#ifndef _DEBUG
	if (isDebug) {
		return;
	}
#else
	if (isDebug) {
		OutputDebugStringA(st.c_str());
	}
#endif

	file << st.c_str() << std::endl;
}

void Log::Message(const std::string& title, std::string& message) {
	MessageBoxA(NULL, message.c_str(), title.c_str(), { MB_OK | MB_SETFOREGROUND | MB_TOPMOST | MB_APPLMODAL });
}

const std::string Log::GetTimeFormatted() const {

	struct tm timeStruct;
	time_t currTime = time(NULL);
	localtime_s(&timeStruct, &currTime);

	char buff[48];
	sprintf_s(buff, "[%02d:%02d:%02d]", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
	return buff;
}
