#pragma once
#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "PhysicalBody.h"

class RigidBody : public PhysicalBody
#ifdef PHYSICS_USE_BULLET
	, public CollisionRigidBody
#endif // !PHYSICS_USE_BULLET
{
public:
	RigidBody(::Transform& targetTransform, const PhysicalMaterial& material, Shape* shape, ShapeComplexType complexType = SIMPLE);
	virtual ~RigidBody();

	virtual void initBody();
	virtual void updateObjectTransform();
	virtual void addToWorld(PhysicalWorld& physicalWorld);
	virtual void removeFromWorld();
	virtual CollisionObject* getCollisionObject() const;
protected:
#ifdef PHYSICS_USE_PHYSX
	PxRigidBody* rawRigidBody = NULL;
#endif // !PHYSICS_USE_PHYSX
};

#endif // !_RIGIDBODY_H_
