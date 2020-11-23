#pragma once
#ifndef _EDITORWINDOW_H_
#define _EDITORWINDOW_H_

#include "InspectorWindow.h"

class EditorWindow : public UIWindow
{
public:
	Material& baseMat;
	bool alwaysShow = false;

	EditorWindow(Object& object, Material& baseMat, string name = "Editor", bool defaultShow = false);
	virtual void onRenderWindow(GUIRenderInfo& info);
};

#endif // !_EDITORWINDOW_H_
