#include "RenderPass.h"

RenderPass::RenderPass(Material& material, RenderTarget & renderTarget) : material(material), renderTarget(renderTarget)
{
}

RenderPass::RenderPass(RenderTarget & renderTarget) : material(Material::nullMaterial), renderTarget(renderTarget)
{
}

void RenderPass::addSourceTexture(const string& name, Texture& sourceTexture)
{
	sourceTextures.insert(pair<string, Texture*>(name, &sourceTexture));
}

void RenderPass::setNextPass(RenderPass & nextPass)
{
	nextRenderPass = &nextPass;
}

void RenderPass::setNextRenderToScreen()
{
	renderToSreen = true;
}

void RenderPass::preRender()
{
	if (renderToSreen)
		RenderTarget::defaultRenderTarget.bindFrame();
	else
		renderTarget.bindFrame();
	if (!material.isNull()) {
		for (auto b = sourceTextures.begin(), e = sourceTextures.end(); b != e; b++)
			material.setTexture(b->first, *b->second);
		if (sourceRenderTarget != NULL)
			sourceRenderTarget->setTexture(material);
		material.initShader();
	}
}

void RenderPass::render(RenderInfo& info)
{
	if (!material.isNull()) {
		if (info.clearMode != NULL)
			glClear(info.clearMode);
		if (!renderTarget.isDefault() && !renderToSreen) {
			renderTarget.resize(info.viewSize.x, info.viewSize.y);
			renderTarget.SetMultisampleFrame();
		}
		glViewport(0, 0, info.viewSize.x, info.viewSize.y);
		glDepthMask(info.depthWrite);
		if (info.alphaTest) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 1.0f);
		}
		else {
			glDisable(GL_ALPHA_TEST);
		}
		for (auto b = info.taskList.begin(), e = info.taskList.end(); b != e; b++) {
			(*b)->setupRenderResource();
			Mesh& mesh = *(Mesh*)(*b)->getShape();
			material.processShader(info.projectionViewMat, transformMat, info.lightSpaceMat, info.cameraLoc, info.cameraDir);
			glDrawArrays(mesh.renderMode, 0, mesh.vertCount);
		}
	}
	else {
		if (info.clearMode != NULL)
			glClear(info.clearMode);
		if (!renderTarget.isDefault() && !renderToSreen) {
			renderTarget.resize(info.viewSize.x, info.viewSize.y);
			renderTarget.SetMultisampleFrame();
		}
		glViewport(0, 0, info.viewSize.x, info.viewSize.y);
		glDepthMask(info.depthWrite);
		if (info.alphaTest) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 1.0f);
		}
		else {
			glDisable(GL_ALPHA_TEST);
		}
		for (auto b = info.taskList.begin(), e = info.taskList.end(); b != e; b++) {
			Render& render = *(Render*)(*b)->getRender();
			Material& mat = *(Material*)(*b)->getMaterial();
			for (auto _b = sourceTextures.begin(), _e = sourceTextures.end(); _b != _e; _b++)
				mat.setTexture(_b->first, *_b->second);
			render.preRender();
			render.render(info);
			render.postRender();
		}
	}
	renderToSreen = false;
}

void RenderPass::postRender()
{
}

Shader * RenderPass::getShader() const
{
	return &material.shader;
}

void RenderPass::setSourceRenderTarget(RenderTarget & renderTarget)
{
	this->sourceRenderTarget = &renderTarget;
}
