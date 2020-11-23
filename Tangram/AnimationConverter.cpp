#define _AFXDLL
#include <afxdlgs.h>
#include <ShlObj.h>
#include "AnimationConverter.h"
#include "Engine.h"
#include "imgui_stdlib.h"
#include "SerializationEditor.h"
#include "SJ2UTF8Table.h"
#include "imgui_internal.h"

namespace ImGui {

	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart*value, bb.Max.y), fg_col);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed*0.333f;
		const float c = speed*0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	}

	bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = abs(ImSin(g.Time*1.8f)*(num_segments - 5));

		const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI*2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);
	}

}

AnimationConverter::AnimationConverter(string name, bool defaultShow)
	: UIWindow(*Engine::getCurrentWorld(), name, defaultShow)
{
	baseMaterial = getAssetByPath<Material>("Engine/Shaders/Toon.mat");
}

AnimationConverter::~AnimationConverter()
{
	if (scene != NULL)
		aiReleaseImport(scene);
	if (fbxInfo != NULL)
		delete fbxInfo;
	if (vmdMotion != NULL)
		delete vmdMotion;
	if (vmdInfo != NULL)
		delete vmdInfo;
}

void AnimationConverter::onRenderWindow(GUIRenderInfo & info)
{
	if (stage == 0) {
		showVmdLoad();
		showPmxLoad();
		if (ImGui::Button("Use Temp Data")) {
			stage = 2;
		}
	}
	else if (stage == 1) {
		ImGui::Spinner("Wait...", 50, 10, ImColor(200, 200, 200));

		DWORD dw;
		dw = WaitForSingleObject(processInfo.hProcess, 0);
		if (dw == WAIT_OBJECT_0) {
			stage = 2;
			filePath = tempPath + "pmx.fbx";
			CloseHandle(processInfo.hThread);
			CloseHandle(processInfo.hProcess);
		}
	}
	else if (stage == 2) {
		if (subStage == -1) {
			subStage = 0;
			thread td = thread([this]() {
				/*if (scene != NULL)
					aiReleaseImport(scene);
				if (fbxInfo != NULL) {
					delete fbxInfo;
					fbxInfo = NULL;
				}
				aiPropertyStore* props = aiCreatePropertyStore();
				aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
				aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
				aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, 1);
				scene = aiImportFileExWithProperties((tempPath + "pmx.fbx").c_str(), aiProcessPreset_TargetRealtime_MaxQuality, NULL, props);
				aiReleasePropertyStore(props);
				subStage = 1;
				if (scene != NULL) {
					fbxInfo = new SerializationInfo[scene->mNumAnimations];
					for (int i = 0; i < scene->mNumAnimations; i++)
						serializeBoneAnimation(fbxInfo[i], *scene->mAnimations[i]);
				}
				subStage = 2;
				if (vmdMotion != NULL)
					delete vmdMotion;
				if (vmdInfo != NULL) {
					delete vmdInfo;
					vmdInfo = NULL;
				}
				vmdMotion = vmd::VmdMotion::LoadFromFile((tempPath + "vmd.vmd").c_str());
				subStage = 3;
				if (vmdMotion != NULL) {
					vmdInfo = new SerializationInfo;
					serializeMorphAnimation(*vmdInfo, *vmdMotion);
				}
				subStage = 4;*/
				if (animationData != NULL)
					delete animationData;
				Importer imp(tempPath + "pmx.fbx");
				if (imp.isLoad()) {
					vector<AnimationClipData*> anims;
					subStage = 1;
					if (imp.getAnimation(anims)) {
						subStage = 2;
						animationData = anims[0];
						vmdMotion = vmd::VmdMotion::LoadFromFile((tempPath + "vmd.vmd").c_str());
						subStage = 3;
						if (vmdMotion != NULL) {
							getMorphAnimation(*animationData, *vmdMotion);
						}
					}
				}
				else
					Console::error("Impoter: %s", imp.getError().c_str());
				subStage = 4;
			});
			td.detach();
		}
		else if (subStage == 4) {
			subStage = -1;
			stage = 3;
		}
		else {
			const char* str;
			switch (subStage)
			{
			case 0: str = "Load Fbx"; break;
			case 1: str = "Load Bone Animation"; break;
			case 2: str = "Load Vmd"; break;
			case 3: str = "Load Morph Animation"; break;
			default: str = "Error"; break;
			}
			ImGui::Text(str);
			ImGui::SameLine();
			ImGui::Spinner("Wait...", 10, 5, ImColor(200, 200, 200));
			ImGui::BufferingBar("Wait...2", subStage / 4.0f, { 200, 5 }, ImColor(20, 20, 20), ImColor(200, 200, 200));
		}
	}
	else if (stage == 3) {
		if (ImGui::Button("Save Animation")) {
			thread td = thread([this]() {
				CFileDialog dialog(false, NULL, NULL, 6UL, _T("charanim(*.charanim)|*.charanim"));
				if (dialog.DoModal() == IDOK) {
					char* s = CT2A(dialog.GetPathName().GetString());
					if (fbxInfo != NULL && vmdInfo != NULL) {
						ofstream ofs = ofstream(s);
						SerializationInfoWriter w = SerializationInfoWriter(ofs);
						ofs << "Motion { ";
						if (fbxInfo != NULL) {
							ofs << fbxInfo->name << ": ";
							w.write(*fbxInfo);
							ofs << ", ";
						}
						if (vmdInfo != NULL) {
							ofs << vmdInfo->name << ": ";
							w.write(*vmdInfo);
						}
						ofs << " }";
						ofs.close();
					}
					if (animationData != NULL) {
						AnimationLoader::writeAnimation(*animationData, s);
					}
					MessageBoxA(NULL, "Complete", "AnimationConverter", MB_OK);
				}
			});
			td.detach();
		}
		ImGui::BeginChild("Scene", { 0, 0 });
		if (scene != NULL && ImGui::CollapsingHeader("FBX")) {
			if (fbxInfo == NULL) {
				if (ImGui::Button("SerializeFbx", { -1, 36 })) {
					fbxInfo = new SerializationInfo[scene->mNumAnimations];
					for (int i = 0; i < scene->mNumAnimations; i++)
						serializeBoneAnimation(fbxInfo[i], *scene->mAnimations[i]);
				}
			}
			else {
				for (int i = 0; i < scene->mNumAnimations; i++) {
					string bstr = "Show ";
					bstr += scene->mAnimations[i]->mName.C_Str();
					if (ImGui::Button(bstr.c_str(), { -1, 36 })) {
						SerializationEditor::showSerializationInfo(info.gui, fbxInfo[i]);
					}
				}
			}
			showAIScene(scene);
		}
		if (vmdMotion != NULL && ImGui::CollapsingHeader("VMD")) {
			if (vmdInfo == NULL) {
				if (ImGui::Button("SerializeVmd", { -1, 36 })) {
					vmdInfo = new SerializationInfo;
					serializeMorphAnimation(*vmdInfo, *vmdMotion);
				}
			}
			else {
				if (ImGui::Button("ShowVmdInfo", { -1, 36 })) {
					SerializationEditor::showSerializationInfo(info.gui, *vmdInfo);
				}
			}
			showVMDMorph(vmdMotion);
		}
		ImGui::EndChild();
	}
}

void AnimationConverter::showFBXLoad()
{
	if (ImGui::Button("ChooseFbx")) {
		thread td = thread([](string* str) {
			CFileDialog dialog(true, NULL, NULL, 6UL, _T("fbx(*.fbx)|*.fbx"));
			if (dialog.DoModal() == IDOK) {
				char* s = CT2A(dialog.GetPathName().GetString());
				*str = s;
			}
		}, &filePath);
		td.detach();
	}
	ImGui::SameLine();
	ImGui::InputText("Fbx", &filePath);
	if (ImGui::Button("LoadFbx", { -1, 40 })) {
		if (scene != NULL)
			aiReleaseImport(scene);
		if (fbxInfo != NULL) {
			delete fbxInfo;
			fbxInfo = NULL;
		}
		aiPropertyStore* props = aiCreatePropertyStore();
		aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
		aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
		aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, 1);
		scene = aiImportFileExWithProperties(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality, NULL, props);
		aiReleasePropertyStore(props);
	}
}

void AnimationConverter::showVmdLoad()
{
	if (ImGui::Button("ChooseVmd")) {
		thread td = thread([](string* str) {
			CFileDialog dialog(true, NULL, NULL, 6UL, _T("vmd(*.vmd)|*.vmd"));
			if (dialog.DoModal() == IDOK) {
				char* s = CT2A(dialog.GetPathName().GetString());
				*str = s;
			}
		}, &vmdPath);
		td.detach();
	}
	ImGui::SameLine();
	ImGui::InputText("Vmd", &vmdPath);
	if (ImGui::Button("LoadVmd", { -1, 40 })) {
		if (vmdMotion != NULL)
			delete vmdMotion;
		if (vmdInfo != NULL) {
			delete vmdInfo;
			vmdInfo = NULL;
		}
		vmdMotion = vmd::VmdMotion::LoadFromFile(vmdPath.c_str());
	}
}

void AnimationConverter::showPmxLoad()
{
	if (ImGui::Button("ChoosePmx")) {
		thread td = thread([](string* str) {
			CFileDialog dialog(true, NULL, NULL, 6UL, _T("pmx(*.pmx)|*.pmx"));
			if (dialog.DoModal() == IDOK) {
				char* s = CT2A(dialog.GetPathName().GetString());
				*str = s;
			}
		}, &pmxPath);
		td.detach();
	}
	ImGui::SameLine();
	ImGui::InputText("Pmx", &pmxPath);
	if (ImGui::Button("Pmx2Fbx", { -1, 40 })) {
		if (!vmdPath.empty() && !pmxPath.empty()) {
			prepare();
		}
	}

	if (ImGui::Button("TargetMatPath")) {
		thread td = thread([](string* str) {
			BROWSEINFOA bi;
			bi.hwndOwner = NULL;
			experimental::filesystem::path p = Engine::windowContext.executionPath;
			LPITEMIDLIST rlist = ILCreateFromPath(p.replace_filename("Content\\").c_str());
			bi.pidlRoot = rlist;
			bi.pszDisplayName = NULL;
			bi.lpszTitle = "Choose Target Folder";
			bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI;
			bi.lpfn = NULL;
			bi.iImage = 0;
			LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
			if (!pidl)
				return;
			char szDisplayName[255];
			SHGetPathFromIDListA(pidl, szDisplayName);
			*str = szDisplayName;
			ILFree(pidl);
		}, &targetMatPath);
		td.detach();
	}
	ImGui::SameLine();
	ImGui::InputText("Target", &targetMatPath);
	if (ImGui::Button("GenerateMaterial", { -1, 40 })) {
		if (!pmxPath.empty()) {
			if (pmxModel != NULL)
				delete pmxModel;
			pmxModel = new pmx::PmxModel;
			ifstream f = ifstream(pmxPath.c_str(), ios::binary | ios::in);
			if (!f.fail()) {
				pmxModel->Read(&f);
				f.close();
			}
			generateMaterial();
		}
	}

	static string toonFile;
	if (ImGui::Button("ChooseToon")) {
		thread td = thread([](string* str) {
			CFileDialog dialog(true, NULL, NULL, 6UL, _T("bmp(*.bmp)|*.bmp"));
			if (dialog.DoModal() == IDOK) {
				char* s = CT2A(dialog.GetPathName().GetString());
				*str = s;
			}
		}, &toonFile);
		td.detach();
	}
	ImGui::SameLine();
	ImGui::InputText("Toon", &toonFile);
	if (ImGui::Button("Test", { -1, 40 })) {
		if (!toonFile.empty()) {
			ToonParameter tp;
			getToonParameter(toonFile, tp);
		}
	}
}

void AnimationConverter::prepare()
{
	if (!PathFileExistsA(tempPath.c_str()))
		CreateDirectoryA(tempPath.c_str(), NULL);
	if (!CopyFileA(pmxPath.c_str(), (tempPath + "pmx.pmx").c_str(), 0))
		Console::error("Fail to copy %s", pmxPath.c_str());
	if (!CopyFileA(vmdPath.c_str(), (tempPath + "vmd.vmd").c_str(), 0))
		Console::error("Fail to copy %s", vmdPath.c_str());
	STARTUPINFOA si = { sizeof(si) };	
	sprintf_s(execCmdChars, "%s %spmx.pmx %svmd.vmd", pmx2fbxPath.c_str(), tempPath.c_str(), tempPath.c_str());
	if (!CreateProcessA(NULL, execCmdChars, NULL, NULL, false, 0, NULL, NULL, &si, &processInfo)) {
		Console::error("Fail to launch %s", pmx2fbxPath.c_str());
		return;
	}
	stage = 1;
}

void AnimationConverter::generateMaterial()
{
	typedef experimental::filesystem::path FPath;
	if (pmxModel == NULL || targetMatPath.empty())
		return;

	SerializationInfo info;
	info.type = "SkeletonMeshActor";
	SerializationInfo& minfo = *info.add("materials");
	minfo.type = "MaterialMap";

	FPath executionPath = Engine::windowContext.executionPath;
	executionPath.replace_filename("");
	FPath folderPath = targetMatPath.substr(executionPath.generic_u8string().length() + 1);
	string folderName = folderPath.filename().generic_u8string();
	FPath ppath = pmxPath;
	set<string> texturePaths;
	for (int i = 0; i < pmxModel->material_count; i++) {
		pmx::PmxMaterial& mat = pmxModel->materials[i];
		string name = sj2utf8(mat.material_name);
		SerializationInfo* mi = minfo.add(name);
		if (mi == NULL)
			continue;
		ToonParameter tp;
		if (mat.common_toon_flag == 0) {
			if (mat.toon_texture_index != -1) {
				FPath p = ppath;
				p.replace_filename(pmxModel->textures[mat.toon_texture_index]).generic_u8string();
				getToonParameter(p.generic_u8string(), tp);
			}
		}

		if (mat.diffuse_texture_index != -1) {
			string& texPath = pmxModel->textures[mat.diffuse_texture_index];
			if (texturePaths.find(texPath) == texturePaths.end())
				texturePaths.insert(texPath);

			FPath texFileName = pmxModel->textures[mat.diffuse_texture_index];
			FPath targetTexPath = folderPath;
			targetTexPath /= folderName + '_' + texFileName.filename().generic_u8string();
			tp.texPath = targetTexPath.generic_u8string();
		}
		MaterialInfo info = MaterialInfo(*baseMaterial);
		info.baseMatPath = "Engine/Shaders/Toon.mat";
		info.setToonParameter(tp);

		string matPath = folderPath.generic_u8string() + '/' + folderName + '_' + name + ".imat";
		
		mi->type = "AssetSearch";
		mi->add("path", matPath);
		mi->add("pathType", "path");

		ofstream f = ofstream(matPath);
		if (!f.fail()) {
			info.write(f);
			f.close();
		}

		/*Console::log("Material: %s", path.c_str());
		Console::log("BaseColor: %f, %f, %f, %f", tp.baseColor.r, tp.baseColor.g, tp.baseColor.b, tp.baseColor.a);
		Console::log("ShadowColor: %f, %f, %f, %f", tp.shadowColor.r, tp.shadowColor.g, tp.shadowColor.b, tp.shadowColor.a);
		Console::log("ShadowCut: %f, ShadowSmooth: %f", tp.shadowCut, tp.shadowSmooth);*/
	}
	for (auto b = texturePaths.begin(), e = texturePaths.end(); b != e; b++) {
		FPath texp = ppath;
		texp.replace_filename(*b);
		string targetTexPath = targetMatPath + '/' + folderName + "_" + FPath(*b).filename().generic_u8string();
		if (CopyFileA(texp.generic_u8string().c_str(), targetTexPath.c_str(), 0))
			Console::log("Copy %s to %s\n", texp.generic_u8string().c_str(), targetTexPath.c_str());
		else
			Console::error("Failed to copy %s to %s\n", texp.generic_u8string().c_str(), targetTexPath.c_str());
	}

	string targetPmxPath = folderPath.generic_u8string() + '/' + folderName + ".pmx";
	info.add("skeletonMesh", targetPmxPath);
	if (CopyFileA(pmxPath.c_str(), targetPmxPath.c_str(), 0))
		Console::log("Copy %s to %s\n", pmxPath.c_str(), targetPmxPath.c_str());
	else
		Console::error("Failed to copy %s to %s\n", pmxPath.c_str(), targetPmxPath.c_str());

	ofstream f = ofstream(targetMatPath + '/' + folderName + ".asset");
	if (!f.fail()) {
		SerializationInfoWriter siw(f);
		siw.write(info);
		f.close();
	}
}

void AnimationConverter::showAIScene(const aiScene * scene)
{
	if (scene == NULL)
		return;
	for (int i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* anim = scene->mAnimations[i];
		ImGui::PushID(i);
		if (ImGui::TreeNode(anim->mName.C_Str())) {
			string animName = anim->mName.C_Str();
			if (ImGui::InputText("Name", &animName))
				anim->mName = animName;
			for (int c = 0; c < anim->mNumChannels; c++) {
				aiNodeAnim* nanim = anim->mChannels[c];
				ImGui::PushID(c);
				if (ImGui::TreeNode(nanim->mNodeName.C_Str())) {
					string nodeName = nanim->mNodeName.C_Str();
					if (ImGui::InputText("Name", &nodeName))
						nanim->mNodeName = nodeName;
					ImGui::Text("PositionKeys: %d", nanim->mNumPositionKeys);
					ImGui::Text("RotationKeys: %d", nanim->mNumRotationKeys);
					ImGui::Text("ScalingKeys: %d", nanim->mNumScalingKeys);
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}

void AnimationConverter::showVMDMorph(const vmd::VmdMotion * vmdMotion)
{
}

void AnimationConverter::serializeNodeAnimation(SerializationInfo & info, const aiNodeAnim & data, float tickPerSec)
{
	info.name = data.mNodeName.C_Str();
	info.type = "Array";
	info.arrayType = "BoneFrame";
	struct Frame
	{
		aiVectorKey* pos;
		aiQuatKey* rot;

		SVector3f getPos()
		{
			return SVector3f(pos->mValue.x, pos->mValue.y, pos->mValue.z);
		}

		SQuaternionf getRot()
		{
			return SQuaternionf(rot->mValue.x, rot->mValue.y, rot->mValue.z, rot->mValue.w);
		}
	};
	map<float, Frame> frames;
	for (int i = 0; i < data.mNumPositionKeys; i++) {
		aiVectorKey* key = &data.mPositionKeys[i];
		frames[key->mTime] = Frame{ key, NULL };
	}
	for (int i = 0; i < data.mNumRotationKeys; i++) {
		aiQuatKey* key = &data.mRotationKeys[i];
		auto iter = frames.find(key->mTime);
		if (iter == frames.end())
			frames[key->mTime] = Frame{ NULL, key };
		else
			iter->second.rot = key;
	}
	for (auto b = frames.begin(), e = frames.end(); b != e; b++) {
		SerializationInfo& fi = *info.push();
		fi.set("frame", b->first / tickPerSec);
		if (b->second.pos != NULL)
			fi.set("position", b->second.getPos());
		if (b->second.rot != NULL)
			fi.set("rotation", b->second.getRot());
	}
}

void AnimationConverter::serializeBoneAnimation(SerializationInfo & info, const aiAnimation & data)
{
	info.name = "boneFrames";
	info.type = "Frames";
	for (int i = 0; i < data.mNumChannels; i++) {
		aiNodeAnim& nodeAnim = *data.mChannels[i];
		serializeNodeAnimation(*info.add(nodeAnim.mNodeName.C_Str()), nodeAnim, data.mTicksPerSecond);
	}
}

void AnimationConverter::serializeMorphAnimation(SerializationInfo & info, const vmd::VmdMotion & data)
{
	info.name = "morphFrames";
	info.type = "Frames";
	for (int i = 0; i < data.face_frames.size(); i++) {
		const vmd::VmdFaceFrame& frame = data.face_frames[i];
		string face_name = sj2utf8(frame.face_name);
		SerializationInfo* mfs = info.get(face_name);
		SerializationInfo* mf;
		if (mfs == NULL) {
			mfs = info.add(face_name);
			mfs->type = "Array";
			mfs->arrayType = "MorphFrame";
			mf = mfs->push();
		}
		else {
			mf = mfs->push();
		}
		mf->set("frame", frame.frame / 30.f);
		mf->set("weight", frame.weight);
	}
}

void AnimationConverter::getMorphAnimation(AnimationClipData & anim, const vmd::VmdMotion & data)
{
	for (int i = 0; i < data.face_frames.size(); i++) {
		const vmd::VmdFaceFrame& frame = data.face_frames[i];
		string face_name = sj2utf8(frame.face_name);
		Curve<float, float>& curve = anim.addMorphAnimationData(face_name);
		curve.insert(frame.frame / 30.f, CurveValue<float>(CurveValue<float>::Linear, frame.weight));
	}
}

bool AnimationConverter::getToonParameter(const string & toonFile, ToonParameter& toonParameter)
{
	Texture2D tex;
	if (!tex.load(toonFile))
		return false;
	unsigned int w = tex.getWidth(), h = tex.getHeight();
	vector<int> zeroCount;
	string text;
	string ztext;
	unsigned int count = 0;
	bool isPure = true;
	for (int i = 0; i < h; i++) {
		Color color, colorL, colorR;
		tex.getPixel(color, i, w / 2);
		if (!tex.getPixel(colorL, i - 1, w / 2)) {
			colorL = color;
		}
		if (!tex.getPixel(colorR, i + 1, w / 2)) {
			colorR = color;
		}
		float diff = fabsf(color.r - colorL.r) + fabsf(color.r - colorR.r) +
			fabsf(color.g - colorL.g) + fabsf(color.g - colorR.g) +
			fabsf(color.b - colorL.b) + fabsf(color.b - colorR.b);
		text += to_string(diff) + " ";
		if (isPure) {
			if (diff == 0)
				count++;
			else if (count != 0) {
				zeroCount.push_back(count);
				count = 1;
				isPure = false;
			}
		}
		else {
			if (diff != 0)
				count++;
			else if (count != 0) {
				zeroCount.push_back(count);
				count = 1;
				isPure = true;
			}
		}
	}
	if (count != 0) {
		zeroCount.push_back(count);
	}
	if (zeroCount.size() < 3) {
		zeroCount.push_back(0);
	}
	while (zeroCount.size() > 3) {
		unsigned int minCount = h, index = 0;
		for (int i = 0; i < zeroCount.size(); i++) {
			if (zeroCount[i] < minCount) {
				minCount = zeroCount[index];
				index = i;
			}
		}
		if (index == 0 || index == zeroCount.size() - 1) {
			zeroCount[zeroCount.size() - 2] += minCount;
			zeroCount.erase(zeroCount.begin() + index);
		}
		else {
			zeroCount[index - 1] += minCount + zeroCount[index + 1];
			zeroCount.erase(zeroCount.begin() + index, zeroCount.begin() + index + 1);
		}
	}
	if (zeroCount.size() < 3) {
		zeroCount.push_back(0);
	}

	Color baseScale, shadowScale;
	tex.getPixel(baseScale, 0, w / 2);
	tex.getPixel(shadowScale, h - 1, w / 2);

	toonParameter.baseColor = baseScale * toonParameter.baseColor.r;
	toonParameter.baseColor.a = 1;
	toonParameter.shadowColor = shadowScale * toonParameter.shadowColor.r;
	toonParameter.shadowColor.a = 1;
	
	toonParameter.shadowCut = (zeroCount[2] == 0 ? 0.0f : (zeroCount[1] / 2.0f + zeroCount[2])) / w;
	toonParameter.shadowSmooth = zeroCount[1] / (float)w;
	return true;
}

AnimationConverter::MaterialInfo::MaterialInfo()
{
}

AnimationConverter::MaterialInfo::MaterialInfo(const Material & mat) : baseMat(&mat)
{
	vector<string> name = split(mat.getShaderName(), '.');
	if (name.empty()) {
		Asset* shd = AssetManager::getAsset("Material", name[0]);
		if (shd != NULL)
			baseMatPath = shd->path;
	}
	isTwoSide = mat.isTwoSide;
	cullFront = mat.cullFront;
	canCastShadow = mat.canCastShadow;
	auto scalarField = mat.getScalarField();
	for (auto b = scalarField.begin(), e = scalarField.end(); b != e; b++) {
		scalars[b->first] = b->second.val;
	}
	auto colorField = mat.getColorField();
	for (auto b = colorField.begin(), e = colorField.end(); b != e; b++) {
		colors[b->first] = b->second.val;
	}
	auto texField = mat.getTextureField();
	for (auto b = texField.begin(), e = texField.end(); b != e; b++) {
		string path = AssetInfo::getPath(b->second.val);
		if (path.empty())
			path = "white";
		textures[b->first] = path;
	}
}

void AnimationConverter::MaterialInfo::setCount(const string & name, float count)
{
	auto iter = counts.find(name);
	if (iter != counts.end())
		iter->second = count;
}

void AnimationConverter::MaterialInfo::setScalar(const string & name, float scalar)
{
	auto iter = scalars.find(name);
	if (iter != scalars.end())
		iter->second = scalar;
}

void AnimationConverter::MaterialInfo::setColor(const string & name, const Color& color)
{
	auto iter = colors.find(name);
	if (iter != colors.end())
		iter->second = color;
}

void AnimationConverter::MaterialInfo::setTexture(const string & name, const string & path)
{
	auto iter = textures.find(name);
	if (iter != textures.end())
		iter->second = path;
}

void AnimationConverter::MaterialInfo::setTexture(const string & name, const Texture & tex)
{
	auto iter = textures.find(name);
	if (iter != textures.end()) {
		string path = AssetInfo::getPath((void*)&tex);
		if (path.empty())
			path = "white";
		iter->second = path;
	}
}

void AnimationConverter::MaterialInfo::setToonParameter(const ToonParameter & toon)
{
	setScalar("highlightCut", 0.999);
	setScalar("rimCut", toon.rimCut);
	setScalar("rimSmooth", toon.rimSmooth);
	setScalar("shadowCut", toon.shadowCut);
	setScalar("shadowSmooth", toon.shadowSmooth);
	setColor("baseColor", toon.baseColor);
	setColor("highlightColor", toon.highlistColor);
	setColor("overColor", toon.overColor);
	setColor("shadowColor", toon.shadowColor);
	setTexture("colorMap", toon.texPath);
	setTexture("normalMap", "Engine/Textures/Default_N.png");
}

ostream & AnimationConverter::MaterialInfo::write(ostream & os)
{
	if (!baseMatPath.empty())
		os << "#material " << baseMatPath << endl;
	os << "#twoside " << (isTwoSide ? "true\n" : "false\n");
	if (cullFront)
		os << "#cullfront true\n";
	os << "#castshadow " << (canCastShadow ? "true\n" : "false\n");
	for (auto b = counts.begin(), e = counts.end(); b != e; b++) {
		os << ("Count " + b->first + ": " + to_string(b->second) + '\n');
	}
	for (auto b = scalars.begin(), e = scalars.end(); b != e; b++) {
		os << ("Scalar " + b->first + ": " + to_string(b->second) + '\n');
	}
	for (auto b = colors.begin(), e = colors.end(); b != e; b++) {
		os << ("Color " + b->first + ": " + to_string(b->second.r) + ", " +
			to_string(b->second.g) + ", " + to_string(b->second.b) + ", " +
			to_string(b->second.a) + '\n');
	}
	for (auto b = textures.begin(), e = textures.end(); b != e; b++) {
		os << ("Texture " + b->first + ": " + (b->second.empty() ? "white" : b->second) + '\n');
	}
	return os;
}
