#include "PostProcessRender.h"
#include "MaterialLoader.h"

Texture2D PostProcessRender::texA = Texture2D(1280, 720, 4);
Texture2D PostProcessRender::texB = Texture2D(1280, 720, 4);
RenderTarget PostProcessRender::bufferA = RenderTarget(1280, 720, 4);
RenderTarget PostProcessRender::bufferB = RenderTarget(1280, 720, 4);
Shader PostProcessRender::defaultShader;
Material PostProcessRender::defaultMaterial = Material(PostProcessRender::defaultShader);
bool PostProcessRender::loadedDefaultResource = false;

PostProcessRender::PostProcessRender(Material& material) : material(material)
{
	loadDefaultResource();
}

PostProcessRender::PostProcessRender(RenderTarget & sourceRenderTarget, Material & material) : material(material)
{
	loadDefaultResource();
	this->sourceRenderTarget = &sourceRenderTarget;
}

void PostProcessRender::preRender()
{
	material.initShader();
}

void PostProcessRender::render(RenderInfo & info)
{
	RenderTarget* tf = &bufferA, *ts = sourceRenderTarget;
	for (int p = 1; p < material.getPassNum(); p++) {
		ts->setTexture(material);
		if (p == material.getPassNum() - 1)
			RenderTarget::defaultRenderTarget.bindFrame();
		else {
			tf->bindFrame();
			tf->SetMultisampleFrame();
		}
		glClear(info.clearMode);
		material.setPass(p);
		material.processShader(info.projectionViewMat, info.transformMat, info.cameraLoc, info.cameraDir);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		if (tf == &bufferA) {
			tf = &bufferB;
			ts = &bufferA;
		}
		else {
			tf = &bufferA;
			ts = &bufferB;
		}
	}
}

IRendering::RenderType PostProcessRender::getRenderType() const
{
	return IRendering::PostProcess;
}

Material * PostProcessRender::getMaterial()
{
	return &material;
}

Shader * PostProcessRender::getShader() const
{
	return &material.shader;
}

void PostProcessRender::setSourceRenderTarget(RenderTarget & renderTarget)
{
	this->sourceRenderTarget = &renderTarget;
}

void PostProcessRender::loadDefaultResource()
{
	if (loadedDefaultResource)
		return;
	if (!MaterialLoader::loadMaterial(defaultMaterial, "Engine/Shaders/PostProcessing.mat")) {
		cout << "Load Engine/Shaders/PostProcessing.mat failed\n";
		throw runtime_error("Load Engine/Shaders/PostProcessing.mat failed");
	}
	bufferA.addTexture("screenMap", texA);
	bufferB.addTexture("screenMap", texB);
	loadedDefaultResource = true;
}
