#include "SkeletonAnimationPipeline.h"

SkeletonAnimationPipe::SkeletonAnimationPipe()
{
}

SkeletonAnimationPipe::~SkeletonAnimationPipe()
{
	if (pose != NULL)
		delete pose;
}

unsigned int SkeletonAnimationPipe::getSourcePipeCount()
{
	return sourcePipes.size();
}

unsigned int SkeletonAnimationPipe::getSourceIndex(const string & name)
{
	auto iter = sourcePipeNames.find(name);
	if (iter == sourcePipeNames.end())
		return -1;
	return iter->second;
}

SkeletonAnimationPipe * SkeletonAnimationPipe::getSourcePipe(unsigned int index)
{
	if (index >= sourcePipes.size())
		return NULL;
	return sourcePipes[index];
}

SkeletonAnimationPipe * SkeletonAnimationPipe::getSourcePipe(const string & name)
{
	return getSourcePipe(getSourceIndex(name));
}

SkeletonAnimationPipe * SkeletonAnimationPipe::getOutPipe()
{
	return outPipe;
}

SkeletonPose * SkeletonAnimationPipe::getPose()
{
	return pose;
}

bool SkeletonAnimationPipe::setSourcePipe(SkeletonAnimationPipe * sourcePipe, unsigned int index)
{
	if (sourcePipe == NULL)
		return false;
	if (index >= sourcePipes.size())
		return false;
	removeSourcePipe(index);
	sourcePipes[index] = sourcePipe;
	sourcePipe->outPipe = this;
	sourcePipe->index = index;
	return true;
}

bool SkeletonAnimationPipe::setOutPipe(SkeletonAnimationPipe * outPipe, unsigned int index)
{
	return outPipe->setSourcePipe(this, index);
}

bool SkeletonAnimationPipe::removeSourcePipe(unsigned int index)
{
	if (index >= sourcePipes.size())
		return false;
	SkeletonAnimationPipe* sp = sourcePipes[index];
	if (sp != NULL) {
		sp->outPipe = NULL;
		sp->index = -1;
		sourcePipes[index] = NULL;
	}
	return true;
}

bool SkeletonAnimationPipe::removeOutPipe()
{
	if (outPipe == NULL)
		return true;
	if (index >= outPipe->sourcePipes.size() && outPipe->sourcePipes[index] != this)
		return false;
	return outPipe->removeSourcePipe(index);
}

bool SkeletonAnimationPipe::connect(SkeletonAnimationPipe & from, SkeletonAnimationPipe & to, unsigned int index)
{
	return to.setSourcePipe(&from, index);
}
