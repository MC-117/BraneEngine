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

	Matrix4f getMatrix(TransformSpace space = RELATE);
	Vector3f getPosition(TransformSpace space = RELATE);
	Quaternionf getRotation(TransformSpace space = RELATE);
	Vector3f getEulerAngle(TransformSpace space = RELATE);
	Vector3f getForward(TransformSpace space = RELATE);
	Vector3f getRightward(TransformSpace space = RELATE);
	Vector3f getUpward(TransformSpace space = RELATE);

	void setMatrix(const Matrix4f& mat, TransformSpace space = RELATE);
	void setPosition(float x, float y, float z, TransformSpace space = RELATE);
	void setRotation(float x, float y, float z, TransformSpace space = RELATE);
	void translate(float x, float y, float z, TransformSpace space = LOCAL);
	void rotate(float x, float y, float z, TransformSpace space = LOCAL);
	void setPosition(const Vector3f& v, TransformSpace space = RELATE);
	void setRotation(const Quaternionf& q, TransformSpace space = RELATE);
	void setRotation(const Vector3f& v, TransformSpace space = RELATE);
	void translate(const Vector3f& v, TransformSpace space = LOCAL);
	void rotate(const Vector3f& v, TransformSpace space = LOCAL);
	void rotate(const Quaternionf& v);
	void setScale(float x, float y, float z);
	void setScale(const Vector3f& v);
	void scaling(float x, float y, float z);
	void scaling(const Vector3f& v);

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