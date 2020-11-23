#pragma once
#ifndef _SKELETONANIMATIONPIPELINE_H_
#define _SKELETONANIMATIONPIPELINE_H_

#include "AnimationClip.h"
#include "SkeletonMesh.h"

class SkeletonAnimationPipeline;

class SkeletonAnimationPipe
{
public:
	SkeletonAnimationPipe();
	virtual ~SkeletonAnimationPipe();

	unsigned int getSourcePipeCount();
	unsigned int getSourceIndex(const string& name);
	SkeletonAnimationPipe* getSourcePipe(unsigned int index);
	SkeletonAnimationPipe* getSourcePipe(const string& name);
	SkeletonAnimationPipe* getOutPipe();
	SkeletonPose* getPose();

	bool setSourcePipe(SkeletonAnimationPipe* sourcePipe, unsigned int index = 0);
	bool setOutPipe(SkeletonAnimationPipe* outPipe, unsigned int index = 0);
	bool removeSourcePipe(unsigned int index = 0);
	bool removeOutPipe();

	virtual void process() = 0;

	static bool connect(SkeletonAnimationPipe& from, SkeletonAnimationPipe& to, unsigned int index);
protected:
	map<string, unsigned int> sourcePipeNames;
	vector<SkeletonAnimationPipe*> sourcePipes;
	SkeletonAnimationPipe* outPipe = NULL;
	unsigned int index = -1;

	SkeletonPose* pose = NULL;

	SkeletonAnimationPipeline* pipeline = NULL;
};

class SkeletonAnimationSource : public SkeletonAnimationPipe
{
public:
protected:
	SkeletonPose _pose;
};

class SkeletonBlendSpace : public SkeletonAnimationSource
{

};

class SkeletonAnimationPipeline
{
public:

};

#endif // !_SKELETONANIMATIONPIPELINE_H_
