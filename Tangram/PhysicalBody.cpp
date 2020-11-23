#include "PhysicalBody.h"
#include "PhysicalWorld.h"

#ifdef PHYSICS_USE_BULLET
PhysicalBodyMotionState::PhysicalBodyMotionState(::Transform & targetTransform, const PTransform& centerOfMassOffset)
	: btDefaultMotionState(targetTransform.getWorldTransform(), centerOfMassOffset), targetTransform(targetTransform)
{
}

void PhysicalBodyMotionState::getWorldTransform(PTransform & worldTrans) const
{
	worldTrans = targetTransform.getWorldTransform() * m_centerOfMassOffset.inverse();
}

void PhysicalBodyMotionState::setWorldTransform(const PTransform & worldTrans)
{
	btDefaultMotionState::setWorldTransform(worldTrans);
	targetTransform.apply(m_graphicsWorldTrans);
}
#endif // PHYSICS_USE_BULLET

PhysicalBody::PhysicalBody(::Transform& targetTransform, const PhysicalMaterial& material, Shape * shape, ShapeComplexType complexType) :
#ifdef PHYSICS_USE_BULLET
	motionState(targetTransform, toPTransform(-shape->getCenter().cwiseProduct(targetTransform.scale))),
#endif // PHYSICS_USE_BULLET
	targetTransform(targetTransform), material(material), shape(shape),
	shapeComplexType(complexType)
{
	switch (complexType)
	{
	case COMPLEX:
		collisionShape = shape->generateComplexCollisionShape(targetTransform.scale);
		break;
	case SIMPLE:
		collisionShape = shape->generateCollisionShape(targetTransform.scale);
		break;
	case NONESHAPE:
		break;
	default:
		break;
	}
	if (complexType != NONESHAPE && collisionShape == NULL) {
		collisionShape = shape->generateCollisionShape();
		shapeComplexType = SIMPLE;
	}
}

PhysicalBody::~PhysicalBody()
{
#ifdef PHYSICS_USE_BULLET
	if (collisionShape != NULL)
		delete collisionShape;
#endif
}

void PhysicalBody::initBody()
{
}

void PhysicalBody::updateObjectTransform()
{
}

void PhysicalBody::handleCollision(const ContactInfo & info)
{
	targetTransform.collisionHappened(info);
}

void PhysicalBody::addToWorld(PhysicalWorld & physicalWorld)
{
}

void PhysicalBody::removeFromWorld()
{
}

CollisionObject * PhysicalBody::getCollisionObject() const
{
	return nullptr;
}

void * PhysicalBody::getSoftObject() const
{
	return nullptr;
}
