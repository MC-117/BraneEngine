#include "PostProcessGraph.h"
#include "SSAOPass.h"
#include "BloomPass.h"
#include "ToneMapPass.h"
#include "BlitPass.h"
#include "DOFPass.h"

PostProcessGraph::~PostProcessGraph()
{
	for (auto b = passes.begin(), e = passes.end(); b != e; b++) {
		delete *b;
	}
}

void PostProcessGraph::addPostProcessPass(PostProcessPass & pass)
{
	passes.push_back(&pass);
	pass.setResource(resource);
	pass.setEnable(true);
}

void PostProcessGraph::addDefaultPasses()
{
	addPostProcessPass(*new SSAOPass());
	addPostProcessPass(*new BloomPass());
	addPostProcessPass(*new DOFPass());
	addPostProcessPass(*new ToneMapPass());
	addPostProcessPass(*new BlitPass());
}

void PostProcessGraph::render(RenderInfo & info)
{
	for (auto b = passes.begin(), e = passes.end(); b != e; b++) {
		(*b)->render(info);
	}
}

void PostProcessGraph::resize(Unit2Di size)
{
	for (auto b = passes.begin(), e = passes.end(); b != e; b++) {
		(*b)->resize(size);
	}
}
