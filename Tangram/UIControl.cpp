#include "UIControl.h"
#include "World.h"

UIControl::UIControl(Object& object, string name, bool defaultShow)
	: object(object), events(this)
{
	this->name = name;
	this->show = defaultShow;
}

void UIControl::render(GUIRenderInfo& info)
{
}

void UIControl::onSceneResize(Unit2Di size)
{
}

void UIControl::onAttech(GUI& gui)
{
}