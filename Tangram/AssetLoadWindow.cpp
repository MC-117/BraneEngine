#define _AFXDLL
#include <afxdlgs.h>
#include "AssetLoadWindow.h"
#include <thread>
#include "Engine.h"

AssetLoadWindow::AssetLoadWindow(Object & object, string name, bool defaultShow) : UIWindow(object, name, defaultShow)
{
}

void AssetLoadWindow::onRenderWindow(GUIRenderInfo & info)
{
	if (ImGui::Button(willLoad ? "File" : "RAM")) {
		willLoad = !willLoad;
	}
	ImGui::SameLine();
	if (willLoad) {
		ImGui::InputText("OBJ Path", path, 200, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("Browse", { -1, 40 })) {
			thread td = thread([](char* str) {
				CFileDialog dialog(true, NULL, NULL, 6UL, _T("obj(*.obj)|*.obj|fbx(*.fbx)|*.fbx|pmx(*.pmx)|*.pmx"));
				if (dialog.DoModal() == IDOK) {
					char* s = CT2A(dialog.GetPathName().GetString());
					strcpy_s(str, 200, s);
				}
			}, path);
			td.detach();
		}
	}
	else {
		vector<string> meshes;
		string mstr;

		for (auto b = MeshAssetInfo::assetInfo.assets.begin(), e = MeshAssetInfo::assetInfo.assets.end(); b != e; b++) {
			meshes.push_back(b->first);
			mstr += b->first + '\0';
		}
		if (selectedSkeletonMesh == NULL) {
			if (ImGui::Combo("Mesh", &selectedMeshId, mstr.c_str())) {
				selectedMesh = (Mesh*)(MeshAssetInfo::assetInfo.assets[meshes[selectedMeshId]]->load());
				selectedSkeletonMesh = dynamic_cast<SkeletonMesh*>(selectedMesh);
			}
		}
		else {
			if (ImGui::Combo("SkeletonMesh", &selectedMeshId, mstr.c_str())) {
				selectedMesh = (Mesh*)(MeshAssetInfo::assetInfo.assets[meshes[selectedMeshId]]->load());
				selectedSkeletonMesh = dynamic_cast<SkeletonMesh*>(selectedMesh);
			}
		}
	}

	vector<string> items;
	string str;
	
	for (auto b = MaterialAssetInfo::assetInfo.assets.begin(), e = MaterialAssetInfo::assetInfo.assets.end(); b != e; b++) {
		items.push_back(b->first);
		str += b->first + '\0';
	}
	if (ImGui::Combo("Base Material", &selectedId, str.c_str())) {
		selectedMat = (Material*)(MaterialAssetInfo::assetInfo.assets[items[selectedId]]->asset[0]);
	}
	ImGui::Checkbox("TwoSides", &twoSides);
	ImGui::Checkbox("CastShadow", &castShadow);
	ImGui::InputText("Display Name", name, 100);
	ImGui::BeginGroup();
	if (selectedSkeletonMesh == NULL) {
		ImGui::Combo("Physical Type", &phyMatCT, "Static\0Dynamic\0NoCollision\0");
		ImGui::InputFloat("Mass", &mass, 0.01);
	}
	ImGui::EndGroup();
	ImVec2 size = { -1, 40 };
	if (ImGui::Button("Load", size)) {
		if (willLoad && strlen(path) == 0) {
			MessageBox(NULL, _T("Path is empty"), _T("Error"), MB_OK);
		}
		else if (strlen(name) == 0) {
			MessageBox(NULL, _T("Name is empty"), _T("Error"), MB_OK);
		}
		else if (Brane::find(typeid(Object).hash_code(), name) != NULL) {
			MessageBox(NULL, _T("Name has used"), _T("Error"), MB_OK);
		}
		else {
			if (willLoad) {
				Mesh* mesh = new Mesh();
				if (Mesh::ObjReader::load(path, *mesh)) {
					Material &mat = *selectedMat;
					mat.setTwoSide(twoSides);
					MeshActor* a = new MeshActor(*mesh, mat, { mass, (PhysicalType)phyMatCT }, *mesh, name);
					a->meshRender.canCastShadow = castShadow;
					world += a;
				}
				else {
					delete mesh;
					MessageBox(NULL, _T("Load Mesh Failed"), _T("Error"), MB_OK);
				}
			}
			else {
				Material &mat = *selectedMat;
				mat.setTwoSide(twoSides);
				if (selectedSkeletonMesh == NULL) {
					MeshActor* a = new MeshActor(*selectedMesh, mat, { mass, (PhysicalType)phyMatCT }, *selectedMesh, name);
					a->meshRender.canCastShadow = castShadow;
					world += a;
				}
				else {
					SkeletonMeshActor* a = new SkeletonMeshActor(*selectedSkeletonMesh, *selectedMat, name);
					a->skeletonMeshRender.canCastShadow = castShadow;
					world += a;
				}
			}
		}
	}
}