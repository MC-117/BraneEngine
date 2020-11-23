#pragma once
#ifndef _ANIMATIONCONVERTER_H_
#define _ANIMATIONCONVERTER_H_

#include "UIWindow.h"
#include "AnimationClip.h"
#include "Serialization.h"
#include <assimp\scene.h>
#include "MMDVmdParser.h"
#include "MMDPmxParser.h"

class AnimationConverter : public UIWindow
{
public:
	struct ToonParameter
	{
		Color baseColor = { 222, 222, 222, 255 };
		Color highlistColor = { 255, 255, 255, 255 };
		Color overColor = { 147, 147, 147, 255 };
		Color shadowColor = { 200, 200, 200, 255 };
		Color rimColor = { 0.196078f, 0.196078f, 0.196078f, 1.0f };
		float shadowCut = 0.5;
		float shadowSmooth = 0;
		float rimCut = 0.8;
		float rimSmooth = 0;
		string texPath;
	};

	struct MaterialInfo
	{
		const Material* baseMat = NULL;
		string baseMatPath;

		bool isTwoSide = false;
		bool cullFront = false;
		bool canCastShadow = true;
		map<string, float> scalars;
		map<string, int> counts;
		map<string, Color> colors;
		map<string, string> textures;

		MaterialInfo();
		MaterialInfo(const Material& mat);

		void setCount(const string& name, float count);
		void setScalar(const string& name, float scalar);
		void setColor(const string& name, const Color& color);
		void setTexture(const string& name, const string& path);
		void setTexture(const string& name, const Texture& tex);

		void setToonParameter(const ToonParameter& toon);

		ostream& write(ostream& os);
	};

	AnimationConverter(string name = "AnimationConverter", bool defaultShow = false);
	~AnimationConverter();

	virtual void onRenderWindow(GUIRenderInfo& info);

	void showFBXLoad();
	void showVmdLoad();
	void showPmxLoad();
	void prepare();
	void generateMaterial();

	static void showAIScene(const aiScene* scene);
	static void showVMDMorph(const vmd::VmdMotion* vmdMotion);
	static void serializeNodeAnimation(SerializationInfo & info, const aiNodeAnim & data, float tickPerSec);
	static void serializeBoneAnimation(SerializationInfo & info, const aiAnimation & data);
	static void serializeMorphAnimation(SerializationInfo & info, const vmd::VmdMotion& data);
	static void getMorphAnimation(AnimationClipData & anim, const vmd::VmdMotion& data);
	static void getCameraAnimation(AnimationClipData & anim, const vmd::VmdMotion& data);
	static bool getToonParameter(const string& toonFile, ToonParameter& toonParameter);
protected:
	Material* baseMaterial = NULL;
	const string pmx2fbxPath = "Tool/AnimationConverter/PMX2FBX/pmx2fbx.exe";
	const string tempPath = "Tool/AnimationConverter/Temp/";
	PROCESS_INFORMATION processInfo;
	char execCmdChars[200];
	int stage = 0, subStage = -1;

	const aiScene* scene = NULL;
	const vmd::VmdMotion* vmdMotion = NULL;
	pmx::PmxModel* pmxModel = NULL;
	SerializationInfo *fbxInfo = NULL, *vmdInfo = NULL;
	AnimationClipData* animationData = NULL;
	string filePath;
	string pmxPath;
	string vmdPath;
	string targetMatPath;
};

#endif // !_ANIMATIONCONVERTER_H_
