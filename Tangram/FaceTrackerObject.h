#pragma once
#ifndef _FACERIGPLUGIN_H_
#define _FACERIGPLUGIN_H_

#include "PluginBase.h"
#include "SkeletonMeshActor.h"
#include <FaceTracker.h>
class FaceRigObject : public Object
{
public:
	enum Part {
		MOUTH, OMOUTH, LEYE, REYE
	};
	SkeletonMeshActor* skeletonMeshActor = NULL;
	Bone* headBone = NULL;
	Bone* lEyeBone = NULL;
	Bone* rEyeBone = NULL;
	FaceTracker faceRig;
	thread* rigThread = NULL;
	bool isStart = false;
	int morphMapping[4] = { -1, -1, -1, -1 };
	float weightClamp[4][2] = {
		{ 50.f, 55.f },
		{ 2.4f, 31.f },
		{ 11.5f, 10.8f },
		{ 11.5f, 10.8f }
	};
	float morphWeights[4][2] = { 0 };
	list<Vector3f> headRots;
	list<Vector3f> lEyeDirs;
	list<Vector3f> rEyeDirs;

	FaceRigObject(const string& name = "FaceRigObject");
	~FaceRigObject();

	virtual void tick(float deltaTime);

	void setMouthHeightMorph(int m);
	void setMouthWidthMorph(int m);
	void setLEyeMorph(int m);
	void setREyeMorph(int m);
	float getMorphWeight(unsigned int i, float data);
	void start();
	void start(int id);
	void start(const string& file);
	void stop();
};

#endif // !_FACERIGPLUGIN_H_
