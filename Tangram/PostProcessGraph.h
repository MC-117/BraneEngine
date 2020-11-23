#pragma once
#ifndef _POSTPROCESSGRAPH_H_
#define _POSTPROCESSGRAPH_H_

#include "PostProcessPass.h"

class PostProcessGraph
{
public:
	PostProcessResource resource;

	list<PostProcessPass*> passes;

	~PostProcessGraph();

	void addPostProcessPass(PostProcessPass& pass);
	void addDefaultPasses();

	virtual void render(RenderInfo& info);
	virtual void resize(Unit2Di size);
};

#endif // !_POSTPROCESSGRAPH_H_
