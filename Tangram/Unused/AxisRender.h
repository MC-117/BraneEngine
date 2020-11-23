#pragma once
#ifndef _AXISRENDER_H_
#define _AXISRENDER_H_

#include "Object.h"
#include "Render.h"

class AxisRender : public Render, public Object
{
public:
	static Shader shader;
	static Material material;
	static bool loaded;
	
	AxisRender();

	virtual IRendering::RenderType getRenderType() const;
	virtual void render(RenderInfo& info);
protected:
	unsigned int vao = 0;
	unsigned int vbo = 0;
	float data[2] = { 20, 20 };
};

#endif // !_AXISRENDER_H_
