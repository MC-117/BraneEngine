#pragma once
#ifndef _SPRINGARM_H_
#define _SPRINGARM_H_

#include "Transform.h"

class SpringArm : public ::Transform
{
public:
	Serialize(SpringArm);

	::Transform handle;

	SpringArm(const string& name = "SpringArm");

	virtual float getSpringLength();
	virtual void setSpringLength(float len);
	virtual float getRadius();
	virtual void setRadius(float radius);

	virtual void tick(float deltaTime);

	virtual void addChild(Object& child);

	static Serializable* instantiate(const SerializationInfo& from);
	virtual bool deserialize(const SerializationInfo& from);
	virtual bool serialize(SerializationInfo& to);
protected:
	float springLength = 50;
	float radius = 2;
};

#endif // !_SPRINGARM_H_
