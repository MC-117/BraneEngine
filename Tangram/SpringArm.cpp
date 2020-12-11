#include "SpringArm.h"
#include "Engine.h"

SpringArm::SpringArm(const string & name) : handle("Handle")
{
	addChild(handle);
}

float SpringArm::getSpringLength()
{
	return springLength;
}

void SpringArm::setSpringLength(float len)
{
	springLength = len;
}

void SpringArm::tick(float deltaTime)
{
	::Transform::tick(deltaTime);
	PxSweepBuffer hit;
	World* world = Engine::getCurrentWorld();
	if (world == NULL)
		return;
	if (world->physicalWorld.physicsScene->sweep(PxSphereGeometry(2), getWorldTransform(),
		toPVec3(-getForward(WORLD)), springLength, hit, PxHitFlag::eDEFAULT, PxQueryFilterData())) {
		if (hit.block.distance > 0)
			handle.setPosition(-hit.block.distance, 0, 0);
		else
			handle.setPosition(-springLength, 0, 0);
	}
	else {
		handle.setPosition(-springLength, 0, 0);
	}
}
