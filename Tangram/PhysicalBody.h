#pragma once
#ifndef _PHYSICALBODY_H_
#define _PHYSICALBODY_H_

#include "Transform.h"
#include "PhysicalMaterial.h"

class PhysicalWorld;

#ifdef PHYSICS_USE_BULLET
class PhysicalBodyMotionState : public btDefaultMotionState {
public:
	::Transform& targetTransform;

	PhysicalBodyMotionState(::Transform& targetTransform, const PTransform& centerOfMassOffset);

	virtual void getWorldTransform(PTransform& worldTrans) const;
	virtual void setWorldTransform(const PTransform& worldTrans);
};
#endif // PHYSICS_USE_BULLET

class PhysicalBody {
public:
	enum BodyType {
		NONE, RIGID, SOFT
	} bodyType = NONE;
	ShapeComplexType shapeComplexType = SIMPLE;
	PhysicalWorld* physicalWorld = NULL;

	::Transform& targetTransform;
	PhysicalMaterial material;
	Shape* shape = NULL;

	CollisionShape* collisionShape = NULL;
#ifdef PHYSICS_USE_BULLET
	PhysicalBodyMotionState motionState;
#endif // !PHYSICS_USE_BULLET

#ifdef PHYSICS_USE_PHYSX
	
#endif // !PHYSICS_USE_PHYSX

	Delegate<void(PhysicalBody*, PhysicalWorld*)> onStepSimulation;

	PhysicalBody(::Transform& targetTransform, const PhysicalMaterial& material, Shape * shape, ShapeComplexType complexType);
	virtual ~PhysicalBody();

	virtual void initBody();
	virtual void updateObjectTransform();
	virtual void handleCollision(const ContactInfo& info);
	virtual void addToWorld(PhysicalWorld& physicalWorld);
	virtual void removeFromWorld();

	virtual CollisionObject* getCollisionObject() const;
	virtual void* getSoftObject() const;
};

#endif // !_PHYSICALBODY_H_