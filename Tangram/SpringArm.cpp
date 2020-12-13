#include "SpringArm.h"
#include "Engine.h"

SerializeInstance(SpringArm);

SpringArm::SpringArm(const string & name) : ::Transform(name), handle("Handle")
{
	addInternalNode(handle);
}

float SpringArm::getSpringLength()
{
	return springLength;
}

void SpringArm::setSpringLength(float len)
{
	springLength = len;
}

float SpringArm::getRadius()
{
	return radius;
}

void SpringArm::setRadius(float radius)
{
	this->radius = radius;
}

void SpringArm::tick(float deltaTime)
{
	::Transform::tick(deltaTime);
	PxSweepBuffer hit;
	World* world = Engine::getCurrentWorld();
	if (world == NULL)
		return;
	if (world->physicalWorld.physicsScene->sweep(PxSphereGeometry(radius), getWorldTransform(),
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

void SpringArm::addChild(Object & child)
{
	handle.addChild(child);
}

Serializable * SpringArm::instantiate(const SerializationInfo & from)
{
	SpringArm* sa = new SpringArm(from.name);
	ChildrenInstantiateObject(from, sa);
	return sa;
}

bool SpringArm::deserialize(const SerializationInfo & from)
{
	return ::Transform::deserialize(from);
}

bool SpringArm::serialize(SerializationInfo & to)
{
	if (!::Transform::serialize(to))
		return false;
	to.type = "SpringArm";
	return true;
}
