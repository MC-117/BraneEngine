#include "UIWindow.h"

UIWindow::UIWindow(Object & object, string name, bool defaultShow) : UIControl(object, name, defaultShow)
{
}

void UIWindow::setBackgroundImg(Texture2D & img)
{
	backgroundTex = &img;
}

void UIWindow::setBackgroundColor(Color color)
{
	this->backgroundColor = color;
}

void UIWindow::setStyle(ImGuiWindowFlags style)
{
	this->style = style;
}

void UIWindow::setForce()
{
	show = true;
	nextForce = true;
}

void UIWindow::render(GUIRenderInfo& info)
{
	int styleCount = styleVars.size() + styleFVars.size();
	for (auto b = styleVars.begin(), e = styleVars.end(); b != e; b++)
		ImGui::PushStyleVar(b->first, b->second);
	for (auto b = styleFVars.begin(), e = styleFVars.end(); b != e; b++)
		ImGui::PushStyleVar(b->first, b->second);
	if (!ImGui::Begin(name.c_str(), showCloseButton ? &show : NULL, (backgroundTex == NULL || blurBackground) ? style : (style | ImGuiWindowFlags_NoBackground))) {
		show = false;
		ImGui::End();
		ImGui::PopStyleVar(styleCount);
		return;
	}
	if (nextForce) {
		ImGui::SetWindowFocus();
		nextForce = false;
	}
	//if (blurBackground) {
	//	if (info.sceneBlurTex != NULL) {
	//		ImGuiStyle& style = ImGui::GetStyle();
	//		ImVec2 pos = ImGui::GetWindowPos();
	//		ImVec2 size = ImGui::GetWindowSize();
	//		ImVec2 b = { pos.x + size.x, pos.y + size.y };
	//		ImGui::GetBackgroundDrawList()->AddImageRounded((void*)info.sceneBlurTex->getTextureID(), pos, b,
	//		{ pos.x / (float)info.viewSize.x, 1 - pos.y / (float)info.viewSize.y },
	//		{ b.x / (float)info.viewSize.x, 1 - b.y / (float)info.viewSize.y }, 0xff666666, style.WindowRounding);//, ImDrawCornerFlags_Top);
	//	}
	//}
	onRenderWindow(info);
	ImGui::End();
	ImGui::PopStyleVar(styleCount);
}

void UIWindow::onRenderWindow(GUIRenderInfo& info)
{
}
