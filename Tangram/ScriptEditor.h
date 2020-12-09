#pragma once
#ifndef _SCRIPTEDITOR_H_
#define _SCRIPTEDITOR_H_

#include "UIWindow.h"
#include "PythonScript.h"
#include "imgui_TextEditor.h"

class ScriptEditor : public UIWindow
{
public:
	ScriptEditor(string name = "ScriptEditor", bool defaultShow = false);
	~ScriptEditor();

	void setScript(PythonScript& script);
	PythonScript* getScript();

	virtual void onRenderWindow(GUIRenderInfo& info);
protected:
	struct CodeTempData
	{
		PythonScript* script = NULL;
		TextEditor* textEditor = NULL;

		CodeTempData(PythonScript& script);
		bool saveCode();
		bool applyCode();
		void release();
	};

	CodeTempData* currentData = NULL;
	unordered_map<PythonScript*, CodeTempData> tempScripts;
};

#endif // !_SCRIPTEDITOR_H_
