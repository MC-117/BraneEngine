#include "PostProcessingCamera.h"
//#include "MaterialLoader.h"

//Shader PostProcessingCamera::postProcessingShader;
//Material PostProcessingCamera::postProcessingMaterial(PostProcessingCamera::postProcessingShader);
//Texture2D PostProcessingCamera::ssaoRandTex;
//bool PostProcessingCamera::loadedDefaultResource = false;

PostProcessingCamera::PostProcessingCamera(string name) : postProcessCameraRender(renderTarget, Material::nullMaterial), Camera::Camera(postProcessCameraRender, name)
{
	//loadDefaultResource();
	renderTarget.addTexture("screenMap", texture);
}

PostProcessingCamera::PostProcessingCamera(Material & material, string name) : postProcessCameraRender(renderTarget, Material::nullMaterial), Camera::Camera(postProcessCameraRender, name)
{
}

void PostProcessingCamera::setVolumnicLight(DirectLight & light)
{
	postProcessCameraRender.setVolumnicLight(light);
}

//void PostProcessingCamera::loadDefaultResource()
//{
//	if (loadedDefaultResource)
//		return;
//	/*if (!Material::MaterialLoader::loadMaterial(postProcessingMaterial, "Engine/Shaders/PostProcessing.mat")) {
//		cout << "Load Engine/Shaders/PostProcessing.mat failed\n";
//		throw runtime_error("Load Engine/Shaders/PostProcessing.mat failed");
//	}
//	if (!ssaoRandTex.load("Engine/Textures/ssaoRand.png")) {
//		cout << "Load Engine/Textures/ssaoRand.png failed\n";
//		throw runtime_error("Engine/Textures/ssaoRand.png failed");
//	}
//	postProcessingMaterial.setTexture("ssaoKernalMap", ssaoRandTex);*/
//	loadedDefaultResource = true;
//}
