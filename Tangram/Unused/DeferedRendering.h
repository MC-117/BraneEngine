#pragma once
#ifndef _DERFEREDRENDERING_H_
#define _DERFEREDRENDERING_H_

#include "RenderingPath.h"

class DerferedRendering : public RenderingPath
{
public:
	RenderTarget positionBuffer = RenderTarget(1280, 720, 3);
	RenderTarget normalBuffer = RenderTarget(1280, 720, 3);
	RenderTarget colorBuffer = RenderTarget(1280, 720, 4);

	DerferedRendering();
	virtual ~DerferedRendering();

	virtual void render();
};

#endif // !_DERFEREDRENDERING_H_
