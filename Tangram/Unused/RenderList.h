#pragma once
#ifndef _RENDERLIST_H_
#define _RENDERLIST_H_

#include "IRendering.h"
#include "Camera.h"

class RenderList
{
public:
	struct _dec
	{
		bool operator() (const float& a, const float& b)
		{
			return a > b;
		}
	};

	multimap<float, IRendering*> lightRender;
	multimap<float, IRendering*> opaqueRender;
	multimap<float, IRendering*, _dec> transparentRender;
	multimap<float, IRendering*> postProcessRender;
	multimap<Shader*, Material*> materials;
	multimap<Shader*, Material*> extraMaterials;

	RenderList();
	virtual ~RenderList();

	bool isValid();
	void setCamera(Camera& camera);

	void add(IRendering& renderer);
	void clear();

protected:
	Camera* camera = NULL;
};

#endif // !_RENDERLIST_H_
