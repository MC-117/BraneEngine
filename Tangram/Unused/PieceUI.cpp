#include "PieceUI.h"
#include "PieceActor.h"

PieceUI::PieceUI(Object& object, Material& material, string name, bool defaultShow) : UIControl(object, name, defaultShow), srcMaterial(material)
{
	checkList = new bool[gridSize.x * gridSize.y];
	memset(checkList, 0, sizeof(bool) * gridSize.x * gridSize.y);
}

PieceUI::PieceUI(Object & object, Material& material, Unit2Di gridSize, string name, bool defaultShow) : UIControl(object, name, defaultShow), srcMaterial(material), gridSize(gridSize)
{
	checkList = new bool[gridSize.x * gridSize.y];
	memset(checkList, 0, sizeof(bool) * gridSize.x * gridSize.y);
}

void PieceUI::render(GUIRenderInfo& info)
{
	ImGui::Begin(name.c_str());
	if (ImGui::SmallButton("x"))
		show = false;
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Generate Piece");
	char str[16];
	for (int r = 0; r < gridSize.x; r++) {
		for (int c = 0; c < gridSize.y; c++) {
			sprintf(str, "%d, %d", r, c);
			ImGui::Checkbox(str, checkList + r * gridSize.y + c);
			ImGui::SameLine();
		}
		ImGui::NewLine();
		ImGui::NewLine();
	}
	ImGui::InputText("Piece Name", pieceName, sizeof(char) * 20);
	ImGui::ColorEdit4("Piece Color", (float*)&pieceColor);
	ImGui::ColorEdit4("Piece Outline Color", (float*)&pieceOutlineColor);
	int strl = strlen(pieceName);
	if(ImGui::Button("Reset"))
		memset(checkList, 0, sizeof(bool) * gridSize.x * gridSize.y);
	ImGui::SameLine();
	if (ImGui::Button("Generate")) {
		if (strl == 0)
			ImGui::OpenPopup("Alert");
		else {
			vector<Unit2Di> vs;
			if (checkVertices(vs)) {
				PieceActor &pA = *new PieceActor(vs, srcMaterial.instantiate(), pieceName);
				pA.meshRender.material.setColor("baseColor", pieceColor);
				pA.meshRender.material.setColor("outlineColor", pieceOutlineColor);
				object.addChild(pA);
			}
			else
				ImGui::OpenPopup("Alert");
		}
	}
	if (ImGui::BeginPopupModal("Alert", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		if (strl == 0)
			ImGui::Text("Need a name for the Piece");
		else
			ImGui::Text("Invalid vertex set");
		if (ImGui::Button("OK"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::End();
}

bool PieceUI::checkVertices(vector<Unit2Di> & res)
{
	vector<Vector2f> vertices, left, right;
	for (int r = 0; r < gridSize.x; r++)
		for (int c = 0; c < gridSize.y; c++)
			if (checkList[r * gridSize.y + c])
				vertices.push_back({ r, c });
	if (vertices.size() < 3)
		return false;
	Vector2f cp = vertices[0];
	auto b = vertices.begin() + 1, e = vertices.end();
	for (; b != e; b++)
		cp = (*b + cp) / 2.0f;
	for (b = vertices.begin(); b != e; b++)
		if (b->y() <= cp.y())
			left.push_back(*b);
		else
			right.push_back(*b);
	sort(left.begin(), left.end(), [](const Vector2f& a, const Vector2f& b)->bool {
		if (a.x() < b.x())
			return true;
		else if (a.x() == b.x())
			return a.y() > b.y();
		return false;
	});
	sort(right.begin(), right.end(), [](const Vector2f& a, const Vector2f& b)->bool {
		if (a.x() > b.x())
			return true;
		else if (a.x() == b.x())
			return a.y() > b.y();
		return false;
	});
	for (b = left.begin(), e = left.end(); b != e; b++)
		res.push_back({ (int)b->x(), (int)b->y() });
	for (b = right.begin(), e = right.end(); b != e; b++)
		res.push_back({ (int)b->x(), (int)b->y() });
	Unit2Di p = res.back();
	for (auto _b = res.begin(), _e = res.end(); _b != _e; _b++) {
		if (_b->x != p.x) {
			double s = abs(_b->slope(p));
			if (s != 1 && s != 0)
				return false;
		}
		p = *_b;
	}
	return true;
}
