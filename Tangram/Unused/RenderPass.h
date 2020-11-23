#pragma once
#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include "Render.h"

class RenderPass : public Render
{
public:
	RenderTarget& renderTarget;
	Material& material;
	RenderPass(Material& material, RenderTarget& renderTarget = RenderTarget::defaultRenderTarget);
	RenderPass(RenderTarget& renderTarget = RenderTarget::defaultRenderTarget);

	void addSourceTexture(const string& name, Texture& sourceTexture);
	void setNextPass(RenderPass& nextPass);

	void setNextRenderToScreen();

	virtual void preRender();
	virtual void render(RenderInfo& info);
	virtual void postRender();
	virtual Shader* getShader() const;
	virtual void setSourceRenderTarget(RenderTarget& renderTarget);

protected:
	map<string, Texture*> sourceTextures;
	RenderTarget* sourceRenderTarget = NULL;
	RenderPass* nextRenderPass = NULL;
	bool renderToSreen = false;
};

#endif // !_RENDERPASS_H_
