#pragma once
#ifndef _GRASSMESHACTOR_H_
#define _GRASSMESHACTOR_H_

#include "MeshActor.h"
#include <random>

class GrassMeshActor : public MeshActor
{
public:
	Serialize(GrassMeshActor);

	float density = 20;
	Vector2i bound = { 100, 100 };
	GrassMeshActor(Mesh& mesh, Material& material, string name = "GrassMeshActor");

	void set(float density, Vector2i& bound);
	void updateData();
	virtual void begin();
	virtual void tick(float delteTime);
	virtual void end();
	virtual void prerender();

	static Serializable* instantiate(const SerializationInfo& from);
	virtual bool deserialize(const SerializationInfo& from);
	virtual bool serialize(SerializationInfo& to);
protected:
	bool update = true;
	unsigned int baseTransID = -1;
	unsigned int transCount = 0;
};

#endif // !_GRASSMESHACTOR_H_
