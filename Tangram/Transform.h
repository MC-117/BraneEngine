#pragma once
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Object.h"
#include "PhysicalMaterial.h"

struct ContactInfo;
class RigidBody;

//struct TransformData
//{
//	Vector3f position;
//	Quaternionf rotation;
//	Vector3f scale;
//};

class Transform : public Object
{
public:
	Serialize(Transform);

	enum AttachRuleEnum
	{
		KeepRelative, KeepWorld, SnapToTarget
	};

	struct AttachRule
	{
		AttachRuleEnum positionRule;
		AttachRuleEnum rotationRule;
		AttachRuleEnum scaleRule;
	};

	Vector3f position = Vector3f(0, 0, 0);
	Quaternionf rotation = Quaternionf::Identity();
	Vector3f scale = Vector3f(1, 1, 1);
	Vector3f forward = { 1, 0, 0 };
	Vector3f rightward = { 0, 1, 0 };
	Vector3f upward = { 0, 0, 1 };

	RigidBody* rigidBody = NULL;

	Transform(string name = "Transform");
	virtual ~Transform();

	virtual void begin();
	virtual void tick(float deltaTime);
	virtual void afterTick();

	virtual void updataRigidBody(Shape* shape = NULL, ShapeComplexType complexType = SIMPLE, const PhysicalMaterial& physicalMaterial = PhysicalMaterial());
	virtual void* getPhysicalBody();
	virtual void setupPhysics(PhysicalWorld& physicalWorld);
	virtual void releasePhysics(PhysicalWorld& physicalWorld);
	virtual void collisionHappened(const ContactInfo& info);

	virtual Matrix4f getMatrix(TransformSpace space = RELATE);
	virtual Vector3f getPosition(TransformSpace space = RELATE);
	virtual Quaternionf getRotation(TransformSpace space = RELATE);
	virtual Vector3f getEulerAngle(TransformSpace space = RELATE);
	virtual Vector3f getForward(TransformSpace space = RELATE);
	virtual Vector3f getRightward(TransformSpace space = RELATE);
	virtual Vector3f getUpward(TransformSpace space = RELATE);

	virtual void setMatrix(const Matrix4f& mat, TransformSpace space = RELATE);
	virtual void setPosition(float x, float y, float z, TransformSpace space = RELATE);
	virtual void setRotation(float x, float y, float z, TransformSpace space = RELATE);
	virtual void translate(float x, float y, float z, TransformSpace space = LOCAL);
	virtual void rotate(float x, float y, float z, TransformSpace space = LOCAL);
	virtual void setPosition(const Vector3f& v, TransformSpace space = RELATE);
	virtual void setRotation(const Quaternionf& q, TransformSpace space = RELATE);
	virtual void setRotation(const Vector3f& v, TransformSpace space = RELATE);
	virtual void translate(const Vector3f& v, TransformSpace space = LOCAL);
	virtual void rotate(const Vector3f& v, TransformSpace space = LOCAL);
	virtual void rotate(const Quaternionf& v);
	virtual void setScale(float x, float y, float z);
	virtual void setScale(const Vector3f& v);
	virtual void scaling(float x, float y, float z);
	virtual void scaling(const Vector3f& v);

	virtual void attachTo(::Transform& target, const AttachRuleEnum& rule = KeepWorld);

	virtual void setParent(Object& parent);

	void apply(const PTransform& tran);
	PTransform getWorldTransform();

	operator PTransform() const;


	Matrix4f& getTransformMat();

	static Serializable* instantiate(const SerializationInfo& from);
	virtual bool deserialize(const SerializationInfo& from);
	virtual bool serialize(SerializationInfo& to);
protected:
	enum UpdateState {
		None = 0, Pos = 1, Sca = 2, Rot = 4
	} updateState = None;
	Matrix4f transformMat = Matrix4f::Identity();

	void updateTransform();
	void invalidate(UpdateState state);
};

#endif // !_TRANSFORM_H_

struct ContactInfo {
	string objectName;
	Vector3f location;
	Vector3f normal;
	Vector3f impact;
	::Transform* otherObject = NULL;
	void* physicalObject = NULL;
};