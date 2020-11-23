#include "CollisionActor.h"
#include "Utility.h"
#include "PhysicalWorld.h"

CollisionActor::CollisionActor(const string & name) : Actor(name)
{
}

CollisionActor::CollisionActor(Shape * collisionShape, const PhysicalMaterial & physicalMaterial, string name)
	: Actor(name)
{
	//updataRigidBody(collisionShape, SIMPLE, physicalMaterial);
}