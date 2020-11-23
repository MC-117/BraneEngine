#include "MaterialWindow.h"
#include "InspectorWindow.h"
#include "Engine.h"

MaterialWindow::MaterialWindow(Material * material, string name, bool defaultShow)
	: UIWindow(world, name, defaultShow), material(material)
{
	events.registerFunc("setMaterial", [](void* win, void* mat) {
		((MaterialWindow*)win)->material = (Material*)mat;
	});
}

void MaterialWindow::setMaterial(Material & material)
{
	this->material = &material;
}

Material * MaterialWindow::getMaterial()
{
	return material;
}

void MaterialWindow::onRenderWindow(GUIRenderInfo & info)
{
	if (material == NULL)
		return;
	if (ImGui::Button("Save Material", { -1, 36 })) {
		string s = AssetInfo::getPath(material);
		if (!s.empty()) {
			if (!Material::MaterialLoader::saveMaterialInstance(s, *material))
#ifdef UNICODE
				MessageBox(NULL, L"Save failed", L"Error", MB_OK);
#else
				MessageBox(NULL, "Save failed", "Error", MB_OK);
#endif // UNICODE
		}
	}
	ImGui::Text("Shader: %s", material->getShaderName().c_str());
	ImGui::Checkbox("TwoSide", &material->isTwoSide);
	ImGui::Checkbox("CastShadow", &material->canCastShadow);
	for (auto b = material->getColorField().begin(), e = material->getColorField().end(); b != e; b++) {
		Color color = b->second.val;
		if (ImGui::ColorEdit4(b->first.c_str(), (float*)&color))
			b->second.val = color;
	}
	for (auto b = material->getScalarField().begin(), e = material->getScalarField().end(); b != e; b++) {
		float val = b->second.val;
		if (ImGui::DragFloat(b->first.c_str(), &val, 0.01))
			b->second.val = val;
	}
	static string choice;
	ImGui::Columns(2, "TextureColumn", false);
	for (auto b = material->getTextureField().begin(), e = material->getTextureField().end(); b != e; b++) {
		if (b->first == "depthMap")
			continue;
		ImGui::PushID(b._Ptr);
		unsigned int id = b->second.val->getTextureID();
		if (ImGui::ImageButton((void*)id, { 64, 64 }, { 0, 1 }, { 1, 0 })) {
			ImGui::OpenPopup("TexSelectPopup");
			choice = b->first;
		}
		ImGui::NextColumn();
		ImGui::Text(b->first.c_str());
		if (ImGui::ArrowButton("AssignTexBT", ImGuiDir_Left)) {
			InspectorWindow *win = dynamic_cast<InspectorWindow*>(info.gui.getUIControl("Inspector"));
			Asset* assignAsset = NULL;
			if (win != NULL)
				assignAsset = win->assignAsset;
			if (assignAsset != NULL && assignAsset->assetInfo.type == "Texture2D") {
				material->setTexture(b->first, *(Texture2D*)assignAsset->load());
			}
		}
		ImGui::NextColumn();
		ImGui::PopID();
	}
	ImGui::Columns();
	if (ImGui::BeginPopup("TexSelectPopup")) {
		for (auto _b = Texture2DAssetInfo::assetInfo.assets.begin(), _e = Texture2DAssetInfo::assetInfo.assets.end();
			_b != _e; _b++) {
			if (_b->second->asset[0] == NULL)
				continue;
			unsigned int id = ((Texture2D*)_b->second->asset[0])->bind();
			if (id == 0)
				id = Texture2D::blackRGBDefaultTex.bind();
			if (ImGui::Selectable(_b->first.c_str(), false, 0, { 0, 60 })) {
				material->setTexture(choice, *((Texture2D*)_b->second->asset[0]));
			}
			ImGui::SameLine(100);
			ImGui::Image((void*)id, { 56, 56 });
		}
		ImGui::EndPopup();
	}
}
