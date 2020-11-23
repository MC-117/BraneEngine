#pragma once
#ifndef _WINDOWBAR_H_
#define _WINDOWBAR_H_

#include "UIWindow.h"

class WindowBar : public UIWindow
{
public:
	WindowBar(Object& object, string name = "WindowBar", bool defaultShow = false);
	virtual void onRenderWindow(GUIRenderInfo& info);
protected:
	bool startMove = false;
};

#endif // !_WINDOWBAR_H_
