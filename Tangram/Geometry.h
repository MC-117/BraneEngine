#pragma once
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Mesh.h"

class Geometry : public Shape
{
public:
	Geometry();
	Geometry(Vector3f controlPointA, Vector3f controlPointB);

	virtual void reshape(Vector3f controlPointA, Vector3f controlPointB);
	virtual Mesh& toMesh(Mesh& mesh);
};

class Box : public Geometry
{
public:
	Serialize(Box);

	Box(Vector3f controlPointA, Vector3f controlPointB);
	Box(float edgeLength = 1, Vector3f center = Vector3f(0, 0, 0));
	Box(float width, float height, float depth, Vector3f center = Vector3f(0, 0, 0));

	static Serializable* instantiate(const SerializationInfo& from);
};

class Sphere : public Geometry
{
public:
	Serialize(Sphere);

	Sphere(Vector3f controlPointA, Vector3f controlPointB);
	Sphere(float radius = 1, Vector3f center = Vector3f(0, 0, 0));

	void setRadius(float radius);

	virtual CollisionShape* generateCollisionShape(const Vector3f& scale = Vector3f(1, 1, 1)) const;
	static Serializable* instantiate(const SerializationInfo& from);
};

class Column : public Geometry
{
public:
	Serialize(Column);

	Column(Vector3f controlPointA, Vector3f controlPointB);
	Column(float radius = 1, float height = 1, Vector3f center = Vector3f(0, 0, 0));

	virtual CollisionShape* generateCollisionShape(const Vector3f& scale = Vector3f(1, 1, 1)) const;
	static Serializable* instantiate(const SerializationInfo& from);
};

class Cone : public Geometry
{
public:
	Serialize(Cone);

	Cone(Vector3f controlPointA, Vector3f controlPointB);
	Cone(float radius = 1, float height = 1, Vector3f center = Vector3f(0, 0, 0));

	virtual CollisionShape* generateCollisionShape(const Vector3f& scale = Vector3f(1, 1, 1)) const;
	static Serializable* instantiate(const SerializationInfo& from);
};

class Capsule : public Geometry
{
public:
	Serialize(Capsule);

	Capsule(Vector3f controlPointA, Vector3f controlPointB);
	Capsule(float radius = 1, float halfLength = 2, Vector3f center = Vector3f(0, 0, 0));

	void setRadius(float radius);
	void setHalfLength(float halfLength);
	virtual CollisionShape* generateCollisionShape(const Vector3f& scale = Vector3f(1, 1, 1)) const;
	static Serializable* instantiate(const SerializationInfo& from);
};

#endif // !_GEOMETRY_H_