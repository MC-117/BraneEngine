#include "EditorWindow.h"
#include "Engine.h"
#include "AssetLoadWindow.h"
#include "ConsoleWindow.h"
#include "AssetBrowser.h"

EditorWindow::EditorWindow(Object & object, Material& baseMat, string name, bool defaultShow) : UIWindow(object, name, defaultShow), baseMat(baseMat)
{
}

void EditorWindow::onRenderWindow(GUIRenderInfo& info)
{
	World* w = dynamic_cast<World*>(&object);
	if (w == NULL)
		return;
	static World& world = *w;
	static GUI& gui = info.gui;
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
	ImGui::Separator();
	ImVec2 size = { ImGui::GetWindowContentRegionWidth() / 2 - 4, 40 };
	if (ImGui::Button("Load", size)) {
		UIControl* uc = gui.getUIControl("LoadAsset");
		bool b = false;
		if (uc != NULL) {
			AssetLoadWindow* alw = dynamic_cast<AssetLoadWindow*>(uc);
			if (alw != NULL) {
				alw->show = true;
			}
			else
				b = true;
		}
		else
			b = true;
		if (b) {
			AssetLoadWindow* win = new AssetLoadWindow(world, "LoadAsset", true);
			world += win;
		}
		//world.loadTransform("Save.world");
		/*CFileDialog dialog(true, _T("*.world"), NULL, 6UL, _T("World(*.world)|*.world|All Files(*.*)|*.*"));
		if (dialog.DoModal() == IDOK) {
		if (!world.loadTransform(string(CT2A(dialog.GetPathName().GetString()))))
		MessageBox(NULL, _T("Load failed"), _T("Alert"), MB_OK);
		}*/
	}
	ImGui::SameLine();
	if (ImGui::Button("Console", size)) {
		UIControl* uc = gui.getUIControl("Console");
		bool b = false;
		if (uc != NULL) {
			ConsoleWindow* win = dynamic_cast<ConsoleWindow*>(uc);
			if (win != NULL) {
				win->show = true;
			}
			else
				b = true;
		}
		else
			b = true;
		if (b) {
			ConsoleWindow* win = new ConsoleWindow(world, "Console", true);
			world += win;
		}
		//world.saveTransform("Save.world");
		/*CFileDialog dialog(false, _T("*.world"), NULL, 6UL, _T("World(*.world)|*.world|All Files(*.*)|*.*"));
		if (dialog.DoModal() == IDOK) {
		if (!world.saveTransform(string(CT2A(dialog.GetPathName().GetString()))))
		MessageBox(NULL, _T("Load failed"), _T("Alert"), MB_OK);
		}*/
	}
	if (ImGui::Button("Engine Browser", size)) {
		UIControl* uc = gui.getUIControl("Engine Browser");
		bool b = false;
		if (uc != NULL) {
			AssetBrowser* win = dynamic_cast<AssetBrowser*>(uc);
			if (win != NULL) {
				win->show = true;
			}
			else
				b = true;
		}
		else
			b = true;
		if (b) {
			AssetBrowser* win = new AssetBrowser(world, "Engine Browser", true);
			win->setCurrentPath("Engine");
			world += win;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Content Browser", size)) {
		UIControl* uc = gui.getUIControl("Content Browser");
		bool b = false;
		if (uc != NULL) {
			AssetBrowser* win = dynamic_cast<AssetBrowser*>(uc);
			if (win != NULL) {
				win->show = true;
			}
			else
				b = true;
		}
		else
			b = true;
		if (b) {
			AssetBrowser* win = new AssetBrowser(world, "Content Browser", true);
			win->setCurrentPath("Content");
			world += win;
		}
	}
	if (!alwaysShow) {
		if (ImGui::Button("Hide UI", size)) {
			world.input.setCursorHidden(true);
			gui.hideAllUIControl();
		}
		ImGui::SameLine();
	}
	if (!show)
		return;
	if (ImGui::Button("Quit Editor", size))
		world.quit();
	static float mainVolume = 1;
	if (ImGui::SliderFloat("Volume", &mainVolume, 0, 1)) {
		world.setMainVolume(mainVolume);
	}
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Objects");
	ImGui::BeginChild("ObjectsView", { -0.1f, -0.1f });
	static Object* selectedObj = NULL;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (selectedObj == &world)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	if (ImGui::TreeNodeEx("world", node_flags)) {
		if (ImGui::IsItemClicked()) {
			selectedObj = &world;
			UIControl* uc = gui.getUIControl("Inspector");
			bool b = false;
			if (uc != NULL) {
				InspectorWindow* ipw = dynamic_cast<InspectorWindow*>(uc);
				if (ipw != NULL) {
					ipw->setTargetObject(world);
					ipw->show = true;
				}
				else
					b = true;
			}
			else
				b = true;
			if (b) {
				InspectorWindow* win = new InspectorWindow(world, "Inspector", true);
				world += win;
			}
		}
		world.traverse([](Object& obj)->bool {
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if (obj.children.empty())
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			if (selectedObj == &obj)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			bool isOpen = ImGui::TreeNodeEx(obj.name.c_str(), node_flags);
			if (ImGui::IsItemClicked()) {
				selectedObj = &obj;
				InspectorWindow* ipw = dynamic_cast<InspectorWindow*>(gui.getUIControl("Inspector"));
				if (ipw == NULL) {
					ipw = new InspectorWindow(world, "Inspector", true);
					world += ipw;

				}
				ipw->setTargetObject(obj);
				ipw->show = true;
			}
			return isOpen;
		}, [](Object& obj, bool run) {
			if (run)
				ImGui::TreePop();
		});
		ImGui::TreePop();
	}
	ImGui::EndChild();
}
