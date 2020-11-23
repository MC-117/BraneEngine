#pragma once
#ifndef _POSTPROCESSRENDER_H_
#define _POSTPROCESSRENDER_H_

#include "Render.h"
#include "RenderTarget.h"

struct PostProcessData
{

};

class PostProcessRender : public Render
{
public:
	static Texture2D texA;
	static Texture2D texB;
	static RenderTarget bufferA;
	static RenderTarget bufferB;
	static Shader defaultShader;
	static Material defaultMaterial;

	Material material;

	PostProcessRender(Material& material = defaultMaterial);
	PostProcessRender(RenderTarget& sourceRenderTarget, Material& material = defaultMaterial);

	virtual void preRender();
	virtual void render(RenderInfo& info);

	virtual IRendering::RenderType getRenderType() const;
	virtual Material* getMaterial();
	virtual Shader* getShader() const;
	virtual void setSourceRenderTarget(RenderTarget& renderTarget);

protected:
	RenderTarget* sourceRenderTarget = NULL;

	static bool loadedDefaultResource;
	void loadDefaultResource();
};

#endif // !_POSTPROCESSRENDER_H_
