#include "ShadowCameraRender.h"

ShadowCameraRender::ShadowCameraRender(Material& shadowMaterial) : CameraRender(depthRenderTarget, shadowMaterial)
{
	renderOrder = -10;
}

void ShadowCameraRender::preRender()
{
	depthRenderTarget.bindFrame();
}

void ShadowCameraRender::render(RenderInfo & info)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
	glViewport(0, 0, size.x, size.y);
	for (auto b = info.taskList.begin(), e = info.taskList.end(); b != e; b++) {
		if (!(*b)->getCanCastShadow())
			continue;
		Shape* shp = (*b)->getShape();
		Mesh* mesh = NULL;
		if (shp == NULL)
			continue;
		material.processShader(projectionViewMat, (*b)->getTransformMatrix(), info.lightSpaceMat, cameraLoc, cameraDir);
		shp->bindShape();
		shp->drawCall();
	}
	//glCullFace(GL_BACK);
}
