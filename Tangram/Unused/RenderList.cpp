#include "RenderList.h"

RenderList::RenderList()
{
}

RenderList::~RenderList()
{
}

bool RenderList::isValid()
{
	return camera != NULL;
}

void RenderList::setCamera(Camera & camera)
{
	this->camera = &camera;
}

void RenderList::add(IRendering & renderer)
{
	Shape* shp = renderer.getShape();
	if (shp != NULL) {
		Vector3f center = ((Render*)renderer.getRender())->transformMat.block(0, 3, 3, 1) + shp->getCenter();
		float d = camera->forward.dot(center);
		if (renderer.getRenderType() == IRendering::PostProcess) {
			postProcessRender.insert(pair<float, IRendering*>(d, &renderer));
		}
		else if (renderer.getRenderType() == IRendering::Light) {
			lightRender.insert(pair<float, IRendering*>(d, &renderer));
		}
		else {
			if (((Render*)renderer.getRender())->renderOrder < 2500)
				opaqueRender.insert(pair<float, IRendering*>(d, &renderer));
			else
				transparentRender.insert(pair<float, IRendering*>(d, &renderer));
			materials.insert(pair<Shader*, Material*>(renderer.getShader(), renderer.getMaterial()));
		}
	}
	else {
		extraMaterials.insert(pair<Shader*, Material*>(renderer.getShader(), renderer.getMaterial()));
	}
}

void RenderList::clear()
{
	opaqueRender.clear();
	transparentRender.clear();
	postProcessRender.clear();
	materials.clear();
	extraMaterials.clear();
}
