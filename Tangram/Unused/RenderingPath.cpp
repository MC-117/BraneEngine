#include "RenderingPath.h"

RenderingPath::RenderingPath()
{
}

RenderingPath::RenderingPath(Camera & camera)
{
	this->camera = &camera;
}

RenderingPath::~RenderingPath()
{
}

void RenderingPath::setCamera(Camera & camera)
{
	this->camera = &camera;
	renderList.setCamera(camera);
}

void RenderingPath::render()
{
}
