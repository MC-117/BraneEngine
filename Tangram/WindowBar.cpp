#include "WindowBar.h"
#include "Engine.h"

WindowBar::WindowBar(Object & object, string name, bool defaultShow) : UIWindow(object, name, defaultShow)
{
	style = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_UnsavedDocument;
	styleVars = { { ImGuiStyleVar_WindowPadding,{ 0, 0 } }, { ImGuiStyleVar_WindowMinSize, { 0, 0 } } };
	//styleFVars = { { ImGuiStyleVar_WindowBorderSize, 0 } };
}

void WindowBar::onRenderWindow(GUIRenderInfo & info)
{
	ImGui::SetWindowPos({ 0, 0 });
	ImGui::SetWindowSize({ float(world.screenSize.x), 30 });
	if (ImGui::IsMouseDragging(0)) {
		if (startMove) {
			Unit2Di pos;
			glfwGetWindowPos((GLFWwindow*)Engine::windowContext.window, &pos.x, &pos.y);
			pos += world.input.getMouseMove();
			glfwSetWindowPos((GLFWwindow*)Engine::windowContext.window, pos.x, pos.y);
		}
		else if (ImGui::IsWindowHovered()) {
			startMove = true;
		}
		else
			startMove = false;
	}
	else
		startMove = false;
	if (ImGui::IsWindowHovered()) {
		if (ImGui::IsMouseDoubleClicked(0))
			Engine::toggleFullscreen();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
	if (ImGui::Button("X", { 30, 20 }))
		world.quit();
	ImGui::SameLine(0, 0);
	if (Engine::windowContext._fullscreen) {
		if (ImGui::Button("o", { 30, 20 }))
			Engine::toggleFullscreen();
	}
	else if (ImGui::Button("O", { 30, 20 }))
		Engine::toggleFullscreen();
	ImGui::SameLine(0, 0);
	if (ImGui::Button("-", { 30, 20 }))
		glfwIconifyWindow((GLFWwindow*)Engine::windowContext.window);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}
