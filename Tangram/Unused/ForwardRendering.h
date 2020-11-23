#pragma once
#ifndef _FORWARDRENDERING_H_
#define _FORWARDRENDERING_H_

#include "RenderingPath.h"

class ForwardRendering : public RenderingPath
{
public:
	Texture2D forwardTex = Texture2D(1280, 720, 4);
	RenderTarget forwardBuffer = RenderTarget(1280, 720, 4, true, 4);

	RenderPass earlyZPass = RenderPass(depthMaterial, forwardBuffer);
	RenderPass opaquePass = RenderPass(forwardBuffer);
	PostProcessPass postProcessPass = PostProcessPass(RenderTarget::defaultRenderTarget);

	ForwardRendering();
	ForwardRendering(Camera& camera);
	virtual ~ForwardRendering();

	virtual void render();

	static Shader depthShader;
	static Material depthMaterial;

protected:
	static bool loaded;
};

#endif // !_FORWARDRENDERING_H_
