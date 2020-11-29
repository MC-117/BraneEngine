#pragma once
#ifndef _UICONTROL_H_
#define _UICONTROL_H_

#include "Object.h"

extern class GUI;
class UIControl;

struct GUIRenderInfo
{
	Unit2Di viewSize;
	Texture* sceneBlurTex;
	RenderCommandList* renderCommandList;
	GUI& gui;
};

struct GUIPostInfo
{
	UIControl* focusControl;
	GUI& gui;
};

class UIControl
{
public:
	string name = "UIControl";
	Object& object;
	bool show = false;
	Events events;

	UIControl(Object& object, string name = "UIControl", bool defaultShow = false);

	virtual void render(GUIRenderInfo& info);
	virtual void onPostAction(GUIPostInfo& info);
	virtual void onSceneResize(Unit2Di size);
	virtual void onAttech(GUI& gui);
};

#endif // !_UICONTROL_H_
