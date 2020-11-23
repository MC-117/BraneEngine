#pragma once
#ifndef _CAPSULEACTOR_H_
#define _CAPSULEACTOR_H_

#include "Geometry.h"
#include "CollisionActor.h"
#include "MeshRender.h"

class CapsuleActor : public CollisionActor
{
public:
	Capsule capsuleShape;
	MeshRender upHemisphereRender;
	MeshRender downHemisphereRender;
	MeshRender columnRender;

	CapsuleActor(const string& name = "CapsuleActor");
	CapsuleActor(const Capsule& capsuleShape, const PhysicalMaterial& physicalMaterial = PhysicalMaterial(), const string& name = "CapsuleActor");

	void setRadius(float radius);
	void setHalfLength(float halfLength);
	float getRadius();
	float getHalfLength();

	virtual void updataRigidBody(Shape* shape = NULL, ShapeComplexType complexType = SIMPLE, const PhysicalMaterial& physicalMaterial = PhysicalMaterial());
	virtual void prerender();
	virtual Render* getRender();
	virtual unsigned int getRenders(vector<Render*>& renders);

	virtual void setHidden(bool value);
	virtual bool isHidden();
protected:
	static Mesh hemisphere;
	static Mesh column;
	static bool isLoadDefaultResource;

	static void loadDefaultResource();
};

#endif // !_CANPSULECOLLISIONACTOR_H_
