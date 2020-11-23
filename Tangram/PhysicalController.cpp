#include "PhysicalController.h"
#include "PhysicalWorld.h"

PhysicalController::PhysicalController(::Transform & targetTransform, Capsule capsule, const PhysicalMaterial& material)
	: capsule(capsule), PhysicalBody(targetTransform, material, &capsule, SIMPLE)
{
	bodyType = RIGID;
}

void PhysicalController::initBody()
{
	if (rawController != NULL) {
		Vector3f pos = targetTransform.getPosition(WORLD);
		rawController->setPosition({ pos.x(), pos.y(), pos.z() });
	}
}

void PhysicalController::updateObjectTransform()
{
	auto pos = rawController->getPosition();
	targetTransform.setPosition(pos.x, pos.y, pos.z, WORLD);
}

void PhysicalController::addToWorld(PhysicalWorld & physicalWorld)
{
#ifdef PHYSICS_USE_PHYSX
	if (rawController == NULL) {
		PxCapsuleControllerDesc desc;
		desc.radius = capsule.getRadius();
		desc.height = capsule.getHeight() - desc.radius * 2;
		desc.userData = this;
		desc.material = physicalWorld.gPhysicsSDK->createMaterial(0.5, 1, 0);
		desc.upDirection = { 0, 0, 1 };
		desc.reportCallback = this;
		desc.maxJumpHeight = 10;
		rawController = physicalWorld.controllerManager->createController(desc);
		if (rawController == NULL)
			throw runtime_error("Create Controller Failed");
		this->physicalWorld = &physicalWorld;
		initBody();
	}
#endif // !PHYSICS_USE_PHYSX
}

void PhysicalController::removeFromWorld()
{
#ifdef PHYSICS_USE_PHYSX
	rawController->release();
	physicalWorld = NULL;
	rawController = NULL;
#endif // !PHYSICS_USE_PHYSX
}

bool PhysicalController::isFly()
{
	return (gravityVelocity.x() != 0 || gravityVelocity.y() != 0 || gravityVelocity.z() != 0);
}

void PhysicalController::move(const Vector3f& position)
{
	moveVector += position;
}

void PhysicalController::jump(float impulse)
{
	Vector3f g = physicalWorld->getGravity();
	g.normalize();
	gravityVelocity -= g * impulse;
}

void PhysicalController::update(float deltaTime)
{
	Vector3f g = physicalWorld->getGravity().cwiseProduct(gravityScale);
	moveVelocity += moveVector + gravityVelocity * deltaTime + 0.5 * g * deltaTime * deltaTime;
	if (!hasCollision)
		gravityVelocity += g * deltaTime;
	if (moveVelocity.x() != 0 || moveVelocity.y() != 0 || moveVelocity.z() != 0) {
		PxControllerFilters filter;
		PxControllerCollisionFlags flag = rawController->move(toPVec3(moveVelocity), 0.01, deltaTime, filter);
		if (flag.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN) ||
			flag.isSet(PxControllerCollisionFlag::eCOLLISION_UP)) {
			gravityVelocity.setZero();
			hasCollision = true;
		}
		else {
			hasCollision = false;
		}
		if (isFly()) {
			if (reservedMoveVelocity.x() == 0 && reservedMoveVelocity.y() == 0 && reservedMoveVelocity.z() == 0) {
				reservedMoveVelocity = moveVelocity;
				Vector3f gi = g.normalized();
				Vector3f sub = gi * gi.dot(reservedMoveVelocity);
				reservedMoveVelocity -= sub;
			}
			moveVelocity = reservedMoveVelocity;
		}
		else {
			reservedMoveVelocity.setZero();
			moveVelocity.setZero();
		}
		moveVector.setZero();
	}
}

static void addForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
{
	/*	if(mode == PxForceMode::eACCELERATION || mode == PxForceMode::eVELOCITY_CHANGE)
	{
	Ps::getFoundation().error(PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__,
	"PxRigidBodyExt::addForce methods do not support eACCELERATION or eVELOCITY_CHANGE modes");
	return;
	}*/

	const PxTransform globalPose = body.getGlobalPose();
	const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

	const PxVec3 torque = (pos - centerOfMass).cross(force);
	body.addForce(force, mode, wakeup);
	body.addTorque(torque, mode, wakeup);
}

static void addForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true)
{
	//transform pos to world space
	const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

	addForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
}

void PhysicalController::onShapeHit(const PxControllerShapeHit & hit)
{
	/*if (hit.actor != NULL) {
		PxRigidDynamic* rig = hit.shape->getActor()->is<PxRigidDynamic>();
		if (rig != NULL && !rig->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC))
			rig->wakeUp();
	}*/
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
	if (actor)
	{
		if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return;

		if (0)
		{
			const PxVec3 p = actor->getGlobalPose().p + hit.dir * 10.0f;

			PxShape* shape;
			actor->getShapes(&shape, 1);
			PxRaycastHit newHit;
			PxU32 n = PxShapeExt::raycast(*shape, *shape->getActor(), p, -hit.dir, 20.0f, PxHitFlag::ePOSITION, 1, &newHit);
			if (n)
			{
				// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
				// useless stress on the solver. It would be possible to enable/disable vertical pushes on
				// particular objects, if the gameplay requires it.
				const PxVec3 upVector = hit.controller->getUpDirection();
				const PxF32 dp = hit.dir.dot(upVector);
				//		shdfnd::printFormatted("%f\n", fabsf(dp));
				if (fabsf(dp)<1e-3f)
					//		if(hit.dir.y==0.0f)
				{
					const PxTransform globalPose = actor->getGlobalPose();
					const PxVec3 localPos = globalPose.transformInv(newHit.position);
					::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
				}
			}
		}

		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);
		//		shdfnd::printFormatted("%f\n", fabsf(dp));
		if (fabsf(dp)<1e-3f)
			//		if(hit.dir.y==0.0f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
			::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
		}
	}
}

void PhysicalController::onControllerHit(const PxControllersHit & hit)
{
}

void PhysicalController::onObstacleHit(const PxControllerObstacleHit & hit)
{
}
