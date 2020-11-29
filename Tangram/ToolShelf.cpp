#include "ToolShelf.h"
#include "Engine.h"

ToolShelf::ToolShelf(string name, bool defaultShow)
	: UIWindow(*Engine::getCurrentWorld(), name, defaultShow)
{
}

void ToolShelf::registTool(UIWindow & tool)
{
	tools.push_back({ &tool, false });
	
}

void ToolShelf::onRenderWindow(GUIRenderInfo & info)
{
	for (int i = 0; i < tools.size(); i++) {
		if (!tools[i].second) {
			info.gui.addUIControl(*tools[i].first);
			tools[i].second = true;
			tools[i].first->show = false;
		}
		if (ImGui::Button(tools[i].first->name.c_str(), { -1, 40 })) {
			tools[i].first->setFocus();
		}
	}
}
