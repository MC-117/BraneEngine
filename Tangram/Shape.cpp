#include "Shape.h"

SerializeInstance(Shape);

Shape::Shape()
{
}

Shape::Shape(const Shape & shape)
{
	renderMode = shape.renderMode;
	bound = shape.bound;
}

Shape::~Shape()
{
}

Shape & Shape::operator=(const Shape & shape)
{
	renderMode = shape.renderMode;
	bound = shape.bound;
	return *this;
}

float * Shape::data() const
{
	return NULL;
}

unsigned int Shape::bindShape()
{
	return 0;
}

void Shape::drawCall()
{
}

Vector3f Shape::getCenter() const
{
	return (bound.col(1) + bound.col(0)) / 2.0f;
}

float Shape::getWidth() const
{
	return abs(bound.col(1).y() - bound.col(0).y());
}

float Shape::getHeight() const
{
	return abs(bound.col(1).z() - bound.col(0).z());
}

float Shape::getDepth() const
{
	return abs(bound.col(1).x() - bound.col(0).x());
}

float Shape::getRadius() const
{
	return abs(bound.col(1).x() - bound.col(0).x()) / 2;
}

CollisionShape * Shape::generateCollisionShape(const Vector3f& scale) const
{
#ifdef PHYSICS_USE_BULLET
	return new btBoxShape(PVec3(getDepth() / 2 * scale.x(), getWidth() / 2 * scale.y(), getHeight() / 2 * scale.z()));
#endif
#ifdef PHYSICS_USE_PHYSX
	return new PxBoxGeometry(PVec3(getDepth() / 2 * scale.x(), getWidth() / 2 * scale.y(), getHeight() / 2 * scale.z()));
#endif
}

CollisionShape * Shape::generateComplexCollisionShape(const Vector3f& scale)
{
	return NULL;
}

Serializable * Shape::instantiate(const SerializationInfo & from)
{
	return new Shape();
}

bool Shape::deserialize(const SerializationInfo & from)
{
	SVector3f minPoint;
	if (!from.get("minPoint", minPoint))
		return false;
	SVector3f maxPoint;
	if (!from.get("maxPoint", maxPoint))
		return false;
	bound << (Vector3f)minPoint, (Vector3f)maxPoint;
	return true;
}

bool Shape::serialize(SerializationInfo & to)
{
	to.set("minPoint", SVector3f(bound.col(0)));
	to.set("maxPoint", SVector3f(bound.col(1)));
	return true;
}
