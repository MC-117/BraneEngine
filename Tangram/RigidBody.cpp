#include "RigidBody.h"
#include "PhysicalWorld.h"
#include "VehicleActor.h"

RigidBody::RigidBody(::Transform& targetTransform, const PhysicalMaterial& material, Shape * shape, ShapeComplexType complexType)
	: PhysicalBody::PhysicalBody(targetTransform, material, shape, complexType)
#ifdef PHYSICS_USE_BULLET
	,CollisionRigidBody::btRigidBody(CollisionRigidBody::btRigidBodyConstructionInfo(material.mass, &motionState, collisionShape))
#endif
{
	bodyType = RIGID;
#ifdef PHYSICS_USE_BULLET
	PVec3 l;
	collisionShape->calculateLocalInertia(material.mass, l);
	setMassProps(material.mass, l);
	switch (material.physicalType)
	{
	case DYNAMIC:
		break;
	case STATIC:
		setCollisionFlags(getCollisionFlags() | CF_KINEMATIC_OBJECT);
		setActivationState(DISABLE_DEACTIVATION);
		break;
	case NOCOLLISIOIN:
		setCollisionFlags(getCollisionFlags() | CF_NO_CONTACT_RESPONSE);
		break;
	default:
		break;
	}
	updateInertiaTensor();
	setRestitution(0);
#endif
#ifdef PHYSICS_USE_PHYSX
	PxMaterial* mat = PhysicalWorld::gPhysicsSDK->createMaterial(1, 0.5, 0);
	Vector3f localSca = targetTransform.getScale();
	switch (material.physicalType)
	{
	case DYNAMIC:
		rawRigidBody = PxCreateDynamic(*PhysicalWorld::gPhysicsSDK, targetTransform.getWorldTransform(),
			*collisionShape, *mat, 1, complexType == SIMPLE ? toPTransform(shape->getCenter().cwiseProduct(localSca)) : PxTransform(PxIDENTITY::PxIdentity));
		rawRigidBody->setMass(material.mass);
		break;
	case STATIC:
		rawRigidBody = PxCreateKinematic(*PhysicalWorld::gPhysicsSDK, targetTransform.getWorldTransform(),
			*collisionShape, *mat, 1, complexType == SIMPLE ? toPTransform(shape->getCenter().cwiseProduct(localSca)) : PxTransform(PxIDENTITY::PxIdentity));
		rawRigidBody->setMass(material.mass);
		rawRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		//rawRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
		break;
	case NOCOLLISIOIN:
		rawRigidBody = PxCreateDynamic(*PhysicalWorld::gPhysicsSDK, targetTransform.getWorldTransform(),
			*collisionShape, *mat, 1, complexType == SIMPLE ? toPTransform(shape->getCenter().cwiseProduct(localSca)) : PxTransform(PxIDENTITY::PxIdentity));
		rawRigidBody->setMass(material.mass);
		rawRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
		break;
	default:
		break;
	}
	if (rawRigidBody != NULL && material.physicalType != NOCOLLISIOIN) {
		rawRigidBody->userData = this;
		rawRigidBody->setActorFlag(PxActorFlag::eVISUALIZATION, true);
		//Get the plane shape so we can set query and simulation filter data.
		PxShape* shapes[1];
		rawRigidBody->getShapes(shapes, 1);

		shapes[0]->setFlag(PxShapeFlag::eVISUALIZATION, true);
		//Get the plane shape so we can set query and simulation fil)

		//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
		PxFilterData qryFilterData;
		qryFilterData.word3 = static_cast<PxU32>(DRIVABLE_SURFACE);
		shapes[0]->setQueryFilterData(qryFilterData);

		//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
		PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		shapes[0]->setSimulationFilterData(groundPlaneSimFilterData);
	}
#endif
}

RigidBody::~RigidBody()
{
#ifdef PHYSICS_USE_PHYSX
	if (rawRigidBody != NULL) {
		rawRigidBody->release();
	}
#endif // PHYSICS_USE_PHYSX
}

void RigidBody::initBody()
{
#ifdef PHYSICS_USE_BULLET
	setWorldTransform(targetTransform.getWorldTransform() * motionState.m_centerOfMassOffset.inverse());
#endif
#ifdef PHYSICS_USE_PHYSX
	rawRigidBody->setGlobalPose(targetTransform.getWorldTransform());
#endif
}

void RigidBody::updateObjectTransform()
{
#ifdef PHYSICS_USE_BULLET
	if (material.physicalType == PhysicalType::DYNAMIC) {
		targetTransform.apply(getWorldTransform());
	}
	else if (material.physicalType == PhysicalType::STATIC) {
		setWorldTransform(targetTransform.getWorldTransform());
	}
#endif
#ifdef PHYSICS_USE_PHYSX
	if (physicalWorld == NULL)
		return;
	if (material.physicalType == PhysicalType::DYNAMIC) {
		targetTransform.apply(rawRigidBody->getGlobalPose());
	}
	else if (material.physicalType == PhysicalType::STATIC) {
		((PxRigidDynamic*)rawRigidBody)->setKinematicTarget(targetTransform.getWorldTransform());
	}
	else if (material.physicalType == PhysicalType::NOCOLLISIOIN)
		rawRigidBody->setGlobalPose(targetTransform.getWorldTransform());
#endif
}

void RigidBody::addToWorld(PhysicalWorld & physicalWorld)
{
#ifdef PHYSICS_USE_BULLET
	switch (bodyType)
	{
	case PhysicalBody::RIGID:
		physicalWorld.physicsScene->addRigidBody((CollisionRigidBody*)(getCollisionObject()));
		break;
	default:
		break;
	}
#endif // !PHYSICS_USE_BULLET
#ifdef PHYSICS_USE_PHYSX
	physicalWorld.physicsScene->addActor(*getCollisionObject());
#endif // !PHYSICS_USE_PHYSX
	this->physicalWorld = &physicalWorld;
	initBody();
}

void RigidBody::removeFromWorld()
{
	if (physicalWorld == NULL)
		return;
#ifdef PHYSICS_USE_BULLET
	switch (bodyType)
	{
	case PhysicalBody::RIGID:
		physicalWorld->physicsScene->removeRigidBody((CollisionRigidBody*)(getCollisionObject()));
		break;
	default:
		break;
	}
#endif // !PHYSICS_USE_BULLET
#ifdef PHYSICS_USE_PHYSX
	if (getCollisionObject() != NULL)
		physicalWorld->physicsScene->removeActor(*getCollisionObject());
#endif // !PHYSICS_USE_PHYSX
	physicalWorld = NULL;
}

CollisionObject * RigidBody::getCollisionObject() const
{
#ifdef PHYSICS_USE_BULLET
	return (CollisionObject*)this;
#endif
#ifdef PHYSICS_USE_PHYSX
	return rawRigidBody;
#endif
}
