#include "FaceTrackerObject.h"

float FaceRigObject::getMorphWeight(unsigned int i, float data)
{
	float w = (data - weightClamp[i][0]) / (weightClamp[i][1] - weightClamp[i][0]);
	return w > 0 ? (w < 1 ? w : 1) : 0;
}

FaceRigObject::FaceRigObject(const string & name) : Object(name)
{
	/*rigThread = new thread([](FaceRigObject* obj) {
		while (obj->isStart) {
			obj->faceRig.update();
		}
	}, this);*/
	events.registerFunc("setTarget", [](void* self, Object* target) {
		((FaceRigObject*)self)->skeletonMeshActor = dynamic_cast<SkeletonMeshActor*>(target);
	});
	events.registerFunc("start", [](void* self) {
		((FaceRigObject*)self)->start();
	});
	events.registerFunc("start", [](void* self, int id) {
		((FaceRigObject*)self)->start(id);
	});
	events.registerFunc("start", [](void* self, string file) {
		((FaceRigObject*)self)->start(file);
	});
	events.registerFunc("stop", [](void* self) {
		((FaceRigObject*)self)->stop();
	});
	events.registerFunc("setMouthWidthMorph", [](void* self, int v) {
		((FaceRigObject*)self)->setMouthWidthMorph(v);
	});
	events.registerFunc("setMouthHeightMorph", [](void* self, int v) {
		((FaceRigObject*)self)->setMouthHeightMorph(v);
	});
	events.registerFunc("setLEyeMorph", [](void* self, int v) {
		((FaceRigObject*)self)->setLEyeMorph(v);
	});
	events.registerFunc("setREyeMorph", [](void* self, int v) {
		((FaceRigObject*)self)->setREyeMorph(v);
	});
	events.registerFunc("setHeadBone", [](void* self, Object* b) {
		((FaceRigObject*)self)->headBone = dynamic_cast<Bone*>(b);
	});
	events.registerFunc("setLEyeBone", [](void* self, Object* b) {
		((FaceRigObject*)self)->lEyeBone = dynamic_cast<Bone*>(b);
	});
	events.registerFunc("setREyeBone", [](void* self, Object* b) {
		((FaceRigObject*)self)->rEyeBone = dynamic_cast<Bone*>(b);
	});
}

FaceRigObject::~FaceRigObject()
{
	stop();
	if (rigThread != NULL)
		delete rigThread;
}

void FaceRigObject::tick(float deltaTime)
{
	Object::tick(deltaTime);
	if (isStart) {
		faceRig.update();
		morphWeights[0][1] = morphWeights[0][0];
		morphWeights[1][1] = morphWeights[1][0];
		morphWeights[2][1] = morphWeights[2][0];
		morphWeights[3][1] = morphWeights[3][0];
		morphWeights[0][0] = getMorphWeight(0, *faceRig.faceParams.mouthWidth);
		morphWeights[1][0] = getMorphWeight(1, *faceRig.faceParams.mouthHeight);
		morphWeights[2][0] = getMorphWeight(2, *faceRig.faceParams.lEyeOpenness);
		morphWeights[3][0] = getMorphWeight(3, *faceRig.faceParams.rEyeOpenness);
		if (morphMapping[0] != -1) {
			skeletonMeshActor->skeletonMeshRender.setMorphWeight(morphMapping[0],
				(morphWeights[0][0] + morphWeights[0][1]) / 2.f);
		}
		if (morphMapping[1] != -1) {
			skeletonMeshActor->skeletonMeshRender.setMorphWeight(morphMapping[1],
				(morphWeights[1][0] + morphWeights[1][1]) / 2.f);
		}
		if (morphMapping[2] != -1) {
			skeletonMeshActor->skeletonMeshRender.setMorphWeight(morphMapping[2],
				(morphWeights[2][0] + morphWeights[2][1]) / 2.f);
		}
		if (morphMapping[3] != -1) {
			skeletonMeshActor->skeletonMeshRender.setMorphWeight(morphMapping[3],
				(morphWeights[3][0] + morphWeights[3][1]) / 2.f);
		}
		if (headBone != NULL) {
			if (headRots.size() >= 5)
				headRots.pop_back();
			headRots.push_front(Vector3f(*faceRig.faceParams.headRotX, -*faceRig.faceParams.headRotY, -*faceRig.faceParams.headRotZ));
			Vector3f headRot = Vector3f(0, 0, 0);
			for (auto b = headRots.begin(), e = headRots.end(); b != e; b++)
				headRot += *b;
			headBone->setRotation(headRot / 5.f);
		}
		if (lEyeBone != NULL) {
			if (lEyeDirs.size() >= 3)
				lEyeDirs.pop_back();
			lEyeDirs.push_front(Vector3f(-*faceRig.faceParams.lEyeDirX, -*faceRig.faceParams.lEyeDirY, *faceRig.faceParams.lEyeDirZ));
			Vector3f lEyeDir = Vector3f(0, 0, 0);
			for (auto b = lEyeDirs.begin(), e = lEyeDirs.end(); b != e; b++)
				lEyeDir += *b;
			Quaternionf q = Quaternionf::FromTwoVectors(Vector3f(0, 0, -1), lEyeDir / 3.f);
			lEyeBone->setRotation(q);
		}
		if (rEyeBone != NULL) {
			if (rEyeDirs.size() >= 3)
				rEyeDirs.pop_back();
			rEyeDirs.push_front(Vector3f(-*faceRig.faceParams.rEyeDirX, -*faceRig.faceParams.rEyeDirY, *faceRig.faceParams.rEyeDirZ));
			Vector3f rEyeDir = Vector3f(0, 0, 0);
			for (auto b = rEyeDirs.begin(), e = rEyeDirs.end(); b != e; b++)
				rEyeDir += *b;
			Quaternionf q = Quaternionf::FromTwoVectors(Vector3f(0, 0, -1), rEyeDir / 3.f);
			rEyeBone->setRotation(q);
		}
	}
}

void FaceRigObject::setMouthHeightMorph(int m)
{
	morphMapping[1] = m;
}

void FaceRigObject::setMouthWidthMorph(int m)
{
	morphMapping[0] = m;
}

void FaceRigObject::setLEyeMorph(int m)
{
	morphMapping[2] = m;
}

void FaceRigObject::setREyeMorph(int m)
{
	morphMapping[3] = m;
}

void FaceRigObject::start()
{
	if (skeletonMeshActor == NULL)
		return;
	if (!faceRig.open(0))
		return;
	isStart = true;
	//rigThread->detach();
}

void FaceRigObject::start(int id)
{
	if (skeletonMeshActor == NULL)
		return;
	if (!faceRig.open(id))
		return;
	isStart = true;
	//rigThread->detach();
}

void FaceRigObject::start(const string & file)
{
	if (skeletonMeshActor == NULL)
		return;
	if (!faceRig.open(file.c_str()))
		return;
	isStart = true;
	//rigThread->detach();
}

void FaceRigObject::stop()
{
	isStart = false;
	faceRig.stop();
}