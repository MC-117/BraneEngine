#include "SkeletonPhysicalAsset.h"
#include "SkeletonMeshActor.h"
#include "CapsuleActor.h"

SerializeInstance(SkeletonPhysicalAsset);

void SkeletonPhysicalAsset::apply(SkeletonMeshActor & actor)
{
	for (auto b = data.begin(), e = data.end(); b != e; b++) {
		Bone* bone = actor.getBone(b->first);
		if (bone != NULL) {
			CollisionActor* ca = NULL;
			if (b->second.type == "Capsule")
				ca = new CapsuleActor(Capsule(b->second.minPoint, b->second.maxPoint), PhysicalMaterial(), b->first + "_Collision");
			if (ca != NULL) {
				bone->addChild(*ca);
				ca->setPosition(b->second.postion);
				ca->setRotation(b->second.rotation);
				ca->setScale(b->second.scale);
			}
		}
	}
}

void SkeletonPhysicalAsset::collect(SkeletonMeshActor & actor)
{
	ObjectIterator iter = ObjectIterator(&actor);
	while (iter.next()) {
		CollisionActor* ca = dynamic_cast<CollisionActor*>(&iter.current());
		if (ca != NULL && ca->rigidBody->shape != NULL) {
			Bone* b = dynamic_cast<Bone*>(ca->parent);
			ShapeData& d = data.insert(pair<string, ShapeData>(b->name, ShapeData())).first->second;
			if (dynamic_cast<CapsuleActor*>(ca) != NULL)
				d.type = "Capsule";
			else
				d.type = "Shape";
			d.minPoint = ca->rigidBody->shape->bound.col(0);
			d.maxPoint = ca->rigidBody->shape->bound.col(1);
			d.postion = ca->position;
			d.rotation = ca->getEulerAngle();
			d.scale = ca->scale;
		}
	}
}

Serializable * SkeletonPhysicalAsset::instantiate(const SerializationInfo & from)
{
	return new SkeletonPhysicalAsset();
}

bool SkeletonPhysicalAsset::deserialize(const SerializationInfo & from)
{
	for (auto b = from.sublists.begin(), e = from.sublists.end(); b != e; b++) {
		ShapeData& sd = data.insert(pair<string, ShapeData>((*b).name, ShapeData())).first->second;
		if (!(*b).get("type", sd.type))
			return false;
		if (!(*b).get("minPoint", sd.minPoint))
			return false;
		if (!(*b).get("maxPoint", sd.maxPoint))
			return false;
		if (!(*b).get("position", sd.postion))
			return false;
		if (!(*b).get("rotation", sd.rotation))
			return false;
		if (!(*b).get("scale", sd.scale))
			return false;
	}
	return true;
}

bool SkeletonPhysicalAsset::serialize(SerializationInfo & to)
{
	to.type = "SkeletonCollision";
	for (auto b = data.begin(), e = data.end(); b != e; b++) {
		SerializationInfo* info = to.add(b->first);
		info->type = "ShapeData";
		info->set("type", b->second.type);
		info->set("minPoint", b->second.minPoint);
		info->set("maxPoint", b->second.maxPoint);
		info->set("postion", b->second.postion);
		info->set("rotation", b->second.rotation);
		info->set("scale", b->second.scale);
	}
	return true;
}
