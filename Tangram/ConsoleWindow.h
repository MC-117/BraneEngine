#pragma once
#ifndef _CONSOLEWINDOW_H_
#define _CONSOLEWINDOW_H_

#include "UIWindow.h"
#include "imgui_TextEditor.h"

class ConsoleWindow : public UIWindow
{
public:
	bool showLog = true, showWarning = true, showError = true;
	bool showPyLog = true, showPyError = true;
	string code;
	TextEditor textEditor;

	ConsoleWindow(Object& object, string name = "Console", bool defaultShow = false);

	virtual void onRenderWindow(GUIRenderInfo& info);

protected:
	int logSize = 0;
	int pyLogSize = 0;

	struct Tag
	{
		string timerName;
		int timeIndex;

		bool operator<(const Tag& t) const
		{
			if (timerName < t.timerName)
				return true;
			else if (timerName == t.timerName)
				return timeIndex < t.timeIndex;
			return false;
		}
	};

	map<Tag, Sampler<float>> timerSamples;
};

#endif // !_CONSOLEWINDOW_H_
