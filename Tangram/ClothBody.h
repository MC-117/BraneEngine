#pragma once
#ifndef _CLOTHBODY_H_
#define _CLOTHBODY_H_

#include "PhysicalBody.h"

class ClothBody : public PhysicalBody
{
public:
	struct ClothParticle
	{
		Vector3f position;
		Vector3f normal;
	};

	ClothBody(::Transform& targetTransform, const PhysicalMaterial& material, Mesh* mesh);
	ClothBody(::Transform& targetTransform, const PhysicalMaterial& material, Mesh* mesh, const vector<unsigned int>& meshPartsIndex);
	virtual ~ClothBody();

	virtual void initBody();
	virtual void updateObjectTransform();
	virtual void addToWorld(PhysicalWorld& physicalWorld);
	virtual void removeFromWorld();
	virtual void* getSoftObject() const;
protected:
#ifdef PHYSICS_USE_PHYSX
	map<MeshPart*, pair<unsigned int, vector<Particle>>> meshParts;
	PCloth* rawClothBody;
#endif // !PHYSICS_USE_PHYSX
};

#endif // !_CLOTHBODY_H_