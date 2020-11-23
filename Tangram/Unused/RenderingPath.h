#pragma once
#ifndef _RENDERINGPATH_H_
#define _RENDERINGPATH_H_

#include "RenderList.h"
#include "GUI.h"
#include "PostProcessPass.h"

class RenderingPath
{
public:

	GUI gui;
	RenderList renderList;

	RenderingPath();
	RenderingPath(Camera& camera);
	virtual ~RenderingPath();

	virtual void setCamera(Camera& camera);

	virtual void render();
protected:
	Camera* camera = NULL;
};

#endif // !_RENDERINGPATH_H_
