#pragma once
#ifndef _SKELETONPHYSICALASSER_H_
#define _SKELETONPHYSICALASSER_H_

#include "Serialization.h"

class SkeletonMeshActor;

struct SkeletonPhysicalAsset : public Serializable
{
	Serialize(SkeletonPhysicalAsset);

	struct ShapeData
	{
		string type;
		SVector3f minPoint;
		SVector3f maxPoint;
		SVector3f postion;
		SVector3f rotation;
		SVector3f scale;
	};

	map<string, ShapeData> data;

	virtual void apply(SkeletonMeshActor& actor);
	virtual void collect(SkeletonMeshActor& actor);

	static Serializable* instantiate(const SerializationInfo& from);
	virtual bool deserialize(const SerializationInfo& from);
	virtual bool serialize(SerializationInfo& to);
};

#endif // !_SKELETONPHYSICALASSER_H_
