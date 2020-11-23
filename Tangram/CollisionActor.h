#pragma once
#ifndef _COLLISIONACTOR_H_
#define _COLLISIONACTOR_H_

#include "Actor.h"
#include "RigidBody.h"

class CollisionActor : public Actor {
public:
	CollisionActor(const string& name = "CollisionActor");
	CollisionActor(Shape* collisionShape, const PhysicalMaterial & physicalMaterial, string name = "CollisionActor");
};

#endif // !_COLLISIONACTOR_H_
