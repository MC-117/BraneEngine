#pragma once
#ifndef _SPRINGARM_H_
#define _SPRINGARM_H_

#include "Transform.h"

class SpringArm : public ::Transform
{
public:
	::Transform handle;

	SpringArm(const string& name = "SpringArm");

	virtual float getSpringLength();
	virtual void setSpringLength(float len);

	virtual void tick(float deltaTime);
protected:
	float springLength = 100;
	float radius = 2;
};

#endif // !_SPRINGARM_H_
