#include "Character.h"
#include "Asset.h"

Character::Character(Capsule capsule, string name)
	: Actor(name), physicalController(*this, capsule)
{
}

void Character::tick(float deltaTime)
{
	Actor::tick(deltaTime);
	physicalController.update(deltaTime);
}

void Character::afterTick()
{
	Actor::afterTick();
	physicalController.updateObjectTransform();
}

void Character::setupPhysics(PhysicalWorld & physicalWorld)
{
	Actor::setupPhysics(physicalWorld);
	if (physicalController.physicalWorld == NULL)
		physicalController.addToWorld(physicalWorld);
}

void Character::releasePhysics(PhysicalWorld & physicalWorld)
{
	Actor::releasePhysics(physicalWorld);
	physicalController.removeFromWorld();
}

bool Character::isFly()
{
	return physicalController.isFly();
}

void Character::move(const Vector3f & v)
{
	physicalController.move(v);
}

void Character::jump(float impulse)
{
	physicalController.jump(impulse);
}
