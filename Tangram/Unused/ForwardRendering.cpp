#include "ForwardRendering.h"
//#include "MaterialLoader.h"
#include "PostProcessRender.h"

Shader ForwardRendering::depthShader = Shader("_Depth", 0);
Material ForwardRendering::depthMaterial = Material(ForwardRendering::depthShader);
bool ForwardRendering::loaded = false;

ForwardRendering::ForwardRendering()
{
	if (!loaded) {
		if (!Material::MaterialLoader::loadMaterial(depthMaterial, "Engine/Shaders/Depth.mat")) {
			cout << "Load Engine/Shaders/Depth.mat failed\n";
			throw runtime_error("Load Engine/Shaders/Depth.mat failed");
		}
		loaded = true;
	}
	forwardBuffer.addTexture("screenMap", forwardTex);
	postProcessPass.setSourceRenderTarget(forwardBuffer);
}

ForwardRendering::ForwardRendering(Camera & camera) : RenderingPath::RenderingPath(camera)
{
	if (!loaded) {
		if (!Material::MaterialLoader::loadMaterial(depthMaterial, "Engine/Shaders/Depth.mat")) {
			cout << "Load Engine/Shaders/Depth.mat failed\n";
			throw runtime_error("Load Engine/Shaders/Depth.mat failed");
		}
		loaded = true;
	}
	forwardBuffer.addTexture("screenMap", forwardTex);
	postProcessPass.setSourceRenderTarget(forwardBuffer);
}

ForwardRendering::~ForwardRendering()
{
}

void ForwardRendering::render()
{
	RenderInfo info;
	info.viewSize = camera->size;
	info.cameraDir = camera->forward;
	info.cameraLoc = camera->position;
	info.projectionViewMat = camera->projectionViewMat;
	if (info.clearMode != NULL)
		glClear(info.clearMode);
	glViewport(0, 0, info.viewSize.x, info.viewSize.y);
	info.clearMode = NULL;
	int pid = 0;
	bool depthWrite = true, alphaTest = false;
	for (auto b = renderList.opaqueRender.begin(), e = renderList.opaqueRender.end(); b != e; b++) {
		info.taskList.push_back(b->second);
	}
	earlyZPass.preRender();
	glColorMask(0, 0, 0, 0);
	earlyZPass.render(info);
	earlyZPass.postRender();
	info.taskList.clear();
	map<Shader*, list<IRendering*>> tasks;
	for (auto b = renderList.opaqueRender.begin(), e = renderList.opaqueRender.end(); b != e; b++) {
		Shader* shd = b->second->getShader();
		if (shd != NULL) {
			auto re = tasks.find(shd);
			list<IRendering*>* l = NULL;
			if (re == tasks.end())
				l = &tasks.insert(pair<Shader*, list<IRendering*>>(shd, list<IRendering*>())).first->second;
			else
				l = &re->second;
			l->push_back(b->second);
		}
	}
	glColorMask(1, 1, 1, 1);
	info.depthWrite = false;
	for (auto b = tasks.begin(), e = tasks.end(); b != e; b++) {
		info.taskList = b->second;
		glDepthFunc(GL_EQUAL);
		((Render*)info.taskList.front()->getRender())->preRender();
		for (auto b = renderList.lightRender.begin(), e = renderList.lightRender.end(); b != e; b++)
			((Render*)b->second->getRender())->preRender();
		opaquePass.render(info);
	}
	
	/*for (auto b = renderList.transparentRender.begin(), e = renderList.transparentRender.end(); b != e; b++) {

	}*/

	
	glDepthFunc(GL_ALWAYS);
	info.depthWrite = false;
	info.clearMode = GL_COLOR_BUFFER_BIT;
	
	postProcessPass.preRender();
	postProcessPass.render(info);
	postProcessPass.postRender();

	gui.render(info);
	//glutSwapBuffers();
}
