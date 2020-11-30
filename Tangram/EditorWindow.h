#pragma once
#ifndef _EDITORWINDOW_H_
#define _EDITORWINDOW_H_

#include "InspectorWindow.h"

class EditorWindow : public UIWindow
{
public:
	Material& baseMat;
	Object* selectedObj = NULL;
	bool alwaysShow = false;
	float mainVolume = 1;
	Object* parentObj = NULL;
	SerializationInfo copyInfo;

	EditorWindow(Object& object, Material& baseMat, string name = "Editor", bool defaultShow = false);
	virtual void onRenderWindow(GUIRenderInfo& info);
	virtual void onPostAction(GUIPostInfo& info);
protected:
	void traverse(Object& obj, GUI& gui, Object*& dragObj, Object*& targetObj);
	void select(Object* obj, GUI& gui);

	string newObjectName;
};

#endif // !_EDITORWINDOW_H_
