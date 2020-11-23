#include "Console.h"
#include <memory>
#include <fstream>

vector<Console::_LOG> Console::logs;
vector<Console::_LOG> Console::pyLogs;
stringstream Console::STDOUT;
stringstream Console::STDERR;
string Console::PYLOGBUF;
string Console::PYERRBUF;
map<string, Timer> Console::timers;
Console Console::console;

Console::Console()
{
	cout.rdbuf(STDOUT.rdbuf());
	cerr.rdbuf(STDERR.rdbuf());
	fstream of;
	of.open("log.txt", ios::out | ios::trunc);
	if (of.is_open()) {
		of.close();
	}
}

void Console::pushLog(vector<_LOG>& buf, LogState state, const std::string fmt_str, va_list ap)
{
	Time dur = Time::duration();
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	buf.push_back({ state, dur, string(formatted.get()) });
	_LOG& l = buf.back();
	string sstr = state == Log_Normal ? "[Log][" : (state == Log_Warning ? "[Warn][" : "[Error][");
	writeToFile(sstr + l.timeStamp.toString() + ']' + l.text);
}

void Console::log(const std::string fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	pushLog(logs, Log_Normal, fmt_str, ap);
	va_end(ap);
}

void Console::pyLog(const std::string fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	pushLog(pyLogs, Log_Normal, fmt_str, ap);
	va_end(ap);
}

void Console::warn(const std::string fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	pushLog(logs, Log_Warning, fmt_str, ap);
	va_end(ap);
}

void Console::error(const std::string fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	pushLog(logs, Log_Error, fmt_str, ap);
	va_end(ap);
}

void Console::pyError(const std::string fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	pushLog(pyLogs, Log_Error, fmt_str, ap);
	va_end(ap);
}

void Console::pyLogWrite(const std::string str)
{
	for (auto b = str.begin(), e = str.end(); b != e; b++) {
		if (*b == '\n')
			pyLogFlush();
		else
			PYLOGBUF += *b;
	}
}

void Console::pyLogFlush()
{
	pyLog(PYLOGBUF.c_str());
	PYLOGBUF.clear();
}

void Console::pyErrWrite(const std::string str)
{
	for (auto b = str.begin(), e = str.end(); b != e; b++) {
		if (*b == '\n')
			pyErrFlush();
		else
			PYERRBUF += *b;
	}
}

void Console::pyErrFlush()
{
	pyError(PYERRBUF.c_str());
	PYERRBUF.clear();
}

Timer & Console::getTimer(const string & name)
{
	auto iter = timers.find(name);
	if (iter == timers.end())
		return timers.insert(pair<string, Timer>(name, Timer())).first->second;
	return iter->second;
}

void Console::writeToFile(const string & str)
{
	fstream of;
	of.open("log.txt", ios::out | ios::app);
	if (!of.is_open())
		return;
	of << str + '\n';
	of.close();
}
