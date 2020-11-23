#pragma once
#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "SkeletonMeshActor.h"
#include "PhysicalController.h"
#include "AnimationStateMachine.h"

class Character : public Actor
{
public:
	PhysicalController physicalController;

	Character(Capsule capsule, string name = "Character");

	virtual void tick(float deltaTime);
	virtual void afterTick();

	virtual void setupPhysics(PhysicalWorld& physicalWorld);
	virtual void releasePhysics(PhysicalWorld& physicalWorld);

	virtual bool isFly();
	virtual void move(const Vector3f& v);
	virtual void jump(float impulse);
};

#endif // !_CHARACTER_H_
