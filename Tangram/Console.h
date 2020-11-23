#pragma once
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "Unit.h"

class Console
{
public:
	enum LogState
	{
		Log_Normal, Log_Warning, Log_Error
	};

	struct _LOG
	{
		LogState state;
		Time timeStamp;
		string text;
	};

	static vector<_LOG> logs;
	static vector<_LOG> pyLogs;
	static stringstream STDOUT;
	static stringstream STDERR;
	static string PYLOGBUF;
	static string PYERRBUF;

	static map<string, Timer> timers;

	Console();
	static void pushLog(vector<_LOG>& buf, LogState state, const std::string fmt_str, va_list ap);
	static void log(const std::string fmt_str, ...);
	static void pyLog(const std::string fmt_str, ...);
	static void warn(const std::string fmt_str, ...);
	static void error(const std::string fmt_str, ...);
	static void pyError(const std::string fmt_str, ...);
	static void pyLogWrite(const std::string str);
	static void pyLogFlush();
	static void pyErrWrite(const std::string str);
	static void pyErrFlush();
	static Timer& getTimer(const string& name);
	static void writeToFile(const string& str);
protected:
	static Console console;
};

#endif // !_CONSOLE_H_
