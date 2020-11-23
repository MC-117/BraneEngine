#include "VehicleActor.h"
#include <vehicle\PxVehicleUtil.h>
//#include "VehicleActor.h"
//#include "Utility.h"
//
//WheelActor::WheelActor(Mesh & mesh, Material & material, WheelInfo & wheelInfo, string path)
//	: mesh(mesh), meshRender(mesh, material), Actor::Actor(path), wheelInfo(wheelInfo)
//{
//}
//
//void WheelActor::setHidden(bool value)
//{
//	meshRender.hidden = value;
//}
//
//bool WheelActor::isHidden()
//{
//	return meshRender.hidden;
//}
//
//void WheelActor::prerender()
//{
//	meshRender.transformMat = transformMat;
//}
//
//Render* WheelActor::getRender()
//{
//	return &meshRender;
//}
//
//unsigned int WheelActor::getRenders(vector<Render*>& renders)
//{
//	renders.push_back(&meshRender);
//	return 1;
//}
//
//void btBetterVehicleRaycaster::setPhysicalWorld(PhysicalWorld & physicalWorld)
//{
//	m_dynamicsWorld = physicalWorld.physicsScene;
//}
//
//void * btBetterVehicleRaycaster::castRay(const PVec3 & from, const PVec3 & to, btVehicleRaycasterResult & result)
//{
//	if (m_dynamicsWorld == NULL)
//		return 0;
//	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
//
//	m_dynamicsWorld->rayTest(from, to, rayCallback);
//
//	if (rayCallback.hasHit())
//	{
//		const CollisionRigidBody* body = CollisionRigidBody::upcast(rayCallback.m_collisionObject);
//		if (body && body->hasContactResponse())
//		{
//			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
//			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
//			result.m_hitNormalInWorld.normalize();
//			result.m_distFraction = rayCallback.m_closestHitFraction;
//			return (void*)body;
//		}
//	}
//	return 0;
//}
//
//VehicleActor::VehicleActor(Mesh & mesh, Material & material, const PhysicalMaterial & physicalMaterial, Shape * collisionShape, ShapeComplexType complexType, string path) :
//	MeshActor(mesh, material, physicalMaterial, *collisionShape, path, complexType),
//	raycaster(), vehicle(vehicleTuning, rigidBody, &raycaster)
//{
//	rigidBody->setActivationState(DISABLE_DEACTIVATION);
//	vehicle.setCoordinateSystem(1, 2, 0);
//
//	rigidBody->onStepSimulation += [](PhysicalBody* body, PhysicalWorld* world) {
//		VehicleActor* v = dynamic_cast<VehicleActor*>(&body->targetTransform);
//		if (v != NULL) {
//			v->applyVehicleControl();
//		}
//	};
//}
//
//void VehicleActor::tick(float deltaTime)
//{
//	MeshActor::tick(deltaTime);
//	for (int i = 0; i < vehicle.getNumWheels(); i++)
//	{
//		vehicle.updateWheelTransform(i, true);
//		wheels[i]->apply(vehicle.getWheelInfo(i).m_worldTransform);
//	}
//}
//
//void VehicleActor::setupPhysics(PhysicalWorld & physicalWorld)
//{
//	MeshActor::setupPhysics(physicalWorld);
//	raycaster.setPhysicalWorld(physicalWorld);
//	physicalWorld.physicsScene->addVehicle(&vehicle);
//	rigidBody->setCenterOfMassTransform(*this);
//	//physicalWorld.physicsScene->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(rigidBody.getBroadphaseHandle(), physicalWorld.physicsScene->getDispatcher());
//	vehicle.resetSuspension();
//	for (int i = 0; i < vehicle.getNumWheels(); i++)
//	{
//		vehicle.updateWheelTransform(i, true);
//	}
//}
//
//void VehicleActor::addWheel(WheelActor & wheel)
//{
//	wheels.push_back(&wheel);
//	wheel.owner = this;
//	WheelInfo& info = wheel.wheelInfo;
//	vehicle.addWheel(toPVec3(info.connectionPoint), toPVec3(info.wheelDirection),
//		toPVec3(info.wheelAxle), info.suspensionRestLength, info.wheelRadius,
//		vehicleTuning, info.isSteerWheel);
//	btWheelInfo& _wheel = vehicle.getWheelInfo(vehicle.getNumWheels() - 1);
//	_wheel.m_suspensionStiffness = info.suspensionStiffness;
//	_wheel.m_wheelsDampingRelaxation = info.suspensionDamping;
//	_wheel.m_wheelsDampingCompression = info.suspensionCompression;
//	_wheel.m_frictionSlip = info.wheelFriction;
//	_wheel.m_rollInfluence = info.rollInfluence;
//	_wheel.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
//	//addChild(wheel);
//}
//
//void VehicleActor::PressAccelerator(bool back)
//{
//	engineForce = (back ? -1 : 1) * maxEngineForce;
//}
//
//void VehicleActor::ReleaseAccelerator()
//{
//	engineForce = 0;
//}
//
//void VehicleActor::PressBrake()
//{
//	breakingForce = maxBreakingForce;
//}
//
//void VehicleActor::ReleaseBrake()
//{
//	breakingForce = 0;
//}
//
//void VehicleActor::SetSteering(float value)
//{
//	float sign = value >= 0 ? 1 : -1;
//	vehicleSteering = sign * min(steeringClamp, abs(value));
//}
//
//float VehicleActor::getEngineForce()
//{
//	return engineForce;
//}
//
//void VehicleActor::applyVehicleControl()
//{
//	for (int i = 0; i < wheels.size(); i++) {
//		if (wheels[i]->wheelInfo.isSteerWheel)
//			vehicle.setSteeringValue(vehicleSteering, i);
//		if (wheels[i]->wheelInfo.isPowerWheel) {
//			vehicle.applyEngineForce(engineForce, i);
//			vehicle.setBrake(breakingForce, i);
//		}
//	}
//}

VehicleSceneQueryData::VehicleSceneQueryData()
	: mNumQueriesPerBatch(0),
	mNumHitResultsPerQuery(0),
	mRaycastResults(NULL),
	mRaycastHitBuffer(NULL),
	mPreFilterShader(NULL),
	mPostFilterShader(NULL)
{
}

VehicleSceneQueryData::~VehicleSceneQueryData()
{
}

VehicleSceneQueryData* VehicleSceneQueryData::allocate
(const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, const PxU32 maxNumHitPointsPerWheel, const PxU32 numVehiclesInBatch,
	PxBatchQueryPreFilterShader preFilterShader, PxBatchQueryPostFilterShader postFilterShader,
	PxAllocatorCallback& allocator)
{
	const PxU32 sqDataSize = ((sizeof(VehicleSceneQueryData) + 15) & ~15);

	const PxU32 maxNumWheels = maxNumVehicles*maxNumWheelsPerVehicle;
	const PxU32 raycastResultSize = ((sizeof(PxRaycastQueryResult)*maxNumWheels + 15) & ~15);
	const PxU32 sweepResultSize = ((sizeof(PxSweepQueryResult)*maxNumWheels + 15) & ~15);

	const PxU32 maxNumHitPoints = maxNumWheels*maxNumHitPointsPerWheel;
	const PxU32 raycastHitSize = ((sizeof(PxRaycastHit)*maxNumHitPoints + 15) & ~15);
	const PxU32 sweepHitSize = ((sizeof(PxSweepHit)*maxNumHitPoints + 15) & ~15);

	const PxU32 size = sqDataSize + raycastResultSize + raycastHitSize + sweepResultSize + sweepHitSize;
	PxU8* buffer = static_cast<PxU8*>(allocator.allocate(size, NULL, NULL, 0));

	VehicleSceneQueryData* sqData = new(buffer) VehicleSceneQueryData();
	sqData->mNumQueriesPerBatch = numVehiclesInBatch*maxNumWheelsPerVehicle;
	sqData->mNumHitResultsPerQuery = maxNumHitPointsPerWheel;
	buffer += sqDataSize;

	sqData->mRaycastResults = reinterpret_cast<PxRaycastQueryResult*>(buffer);
	buffer += raycastResultSize;

	sqData->mRaycastHitBuffer = reinterpret_cast<PxRaycastHit*>(buffer);
	buffer += raycastHitSize;

	sqData->mSweepResults = reinterpret_cast<PxSweepQueryResult*>(buffer);
	buffer += sweepResultSize;

	sqData->mSweepHitBuffer = reinterpret_cast<PxSweepHit*>(buffer);
	buffer += sweepHitSize;

	for (PxU32 i = 0; i < maxNumWheels; i++)
	{
		new(sqData->mRaycastResults + i) PxRaycastQueryResult();
		new(sqData->mSweepResults + i) PxSweepQueryResult();
	}

	for (PxU32 i = 0; i < maxNumHitPoints; i++)
	{
		new(sqData->mRaycastHitBuffer + i) PxRaycastHit();
		new(sqData->mSweepHitBuffer + i) PxSweepHit();
	}

	sqData->mPreFilterShader = preFilterShader;
	sqData->mPostFilterShader = postFilterShader;

	return sqData;
}

void VehicleSceneQueryData::free(PxAllocatorCallback& allocator)
{
	allocator.deallocate(this);
}

PxBatchQuery* VehicleSceneQueryData::setUpBatchedSceneQuery(const PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene)
{
	const PxU32 maxNumQueriesInBatch = vehicleSceneQueryData.mNumQueriesPerBatch;
	const PxU32 maxNumHitResultsInBatch = vehicleSceneQueryData.mNumQueriesPerBatch*vehicleSceneQueryData.mNumHitResultsPerQuery;

	PxBatchQueryDesc sqDesc(maxNumQueriesInBatch, maxNumQueriesInBatch, 0);

	sqDesc.queryMemory.userRaycastResultBuffer = vehicleSceneQueryData.mRaycastResults + batchId*maxNumQueriesInBatch;
	sqDesc.queryMemory.userRaycastTouchBuffer = vehicleSceneQueryData.mRaycastHitBuffer + batchId*maxNumHitResultsInBatch;
	sqDesc.queryMemory.raycastTouchBufferSize = maxNumHitResultsInBatch;

	sqDesc.queryMemory.userSweepResultBuffer = vehicleSceneQueryData.mSweepResults + batchId*maxNumQueriesInBatch;
	sqDesc.queryMemory.userSweepTouchBuffer = vehicleSceneQueryData.mSweepHitBuffer + batchId*maxNumHitResultsInBatch;
	sqDesc.queryMemory.sweepTouchBufferSize = maxNumHitResultsInBatch;

	sqDesc.preFilterShader = vehicleSceneQueryData.mPreFilterShader;

	sqDesc.postFilterShader = vehicleSceneQueryData.mPostFilterShader;

	return scene->createBatchQuery(sqDesc);
}

PxRaycastQueryResult* VehicleSceneQueryData::getRaycastQueryResultBuffer(const PxU32 batchId)
{
	return (mRaycastResults + batchId*mNumQueriesPerBatch);
}

PxSweepQueryResult* VehicleSceneQueryData::getSweepQueryResultBuffer(const PxU32 batchId)
{
	return (mSweepResults + batchId*mNumQueriesPerBatch);
}


PxU32 VehicleSceneQueryData::getQueryResultBufferSize() const
{
	return mNumQueriesPerBatch;
}

VehicleActor::VehicleActor(VehicleInfo & vehicleInfo, Mesh & mesh, Material & material, Shape * chassisCollision, ShapeComplexType complexType, string name)
	: mesh(mesh), meshRender(mesh, material), chassisCollision(chassisCollision), chassisCollisioncComplexType(complexType), vehicleInfo(vehicleInfo), Actor::Actor(name)
{
	for (int i = 0; i < 4; i++) {
		WheelInfo& wi = vehicleInfo.wheelInfos[i];
		wheelRenders.push_back(new MeshRender(*wi.mesh, wi.material == NULL ? Material::defaultMaterial : *wi.material));
	}
	setup();
}

VehicleActor::~VehicleActor()
{
	for (int i = 0; i < wheelRenders.size(); i++) {
		delete wheelRenders[i];
	}
	gVehicle4W->release();
	rawVehicleActor->release();
}

static PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
{
	//NORMAL,	WORN
	{ 1.00f}//,		0.1f }//TARMAC
};

PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs
(const PxMaterial* defaultMaterial)
{
	PxVehicleDrivableSurfaceType surfaceTypes[1];
	surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

	const PxMaterial* surfaceMaterials[1];
	surfaceMaterials[0] = defaultMaterial;

	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES,
			MAX_NUM_SURFACE_TYPES);

	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES,
		surfaceMaterials, surfaceTypes);

	for (PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
	{
		for (PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
		{
			surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
		}
	}
	return surfaceTirePairs;
}

PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension query.
	//filterData1 is the shape potentially hit by the query.
	PX_UNUSED(filterData0);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
}

PxQueryHitType::Enum WheelSceneQueryPostFilterBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	const PxQueryHit& hit)
{
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	if ((static_cast<const PxSweepHit&>(hit)).hadInitialOverlap())
		return PxQueryHitType::eNONE;
	return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum WheelSceneQueryPreFilterNonBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension query.
	//filterData1 is the shape potentially hit by the query.
	PX_UNUSED(filterData0);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eTOUCH);
}

PxQueryHitType::Enum WheelSceneQueryPostFilterNonBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	const PxQueryHit& hit)
{
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	if ((static_cast<const PxSweepHit&>(hit)).hadInitialOverlap())
		return PxQueryHitType::eNONE;
	return PxQueryHitType::eTOUCH;
}

void VehicleActor::tick(float deltaTime)
{
	Actor::tick(deltaTime);
}

void VehicleActor::prerender()
{
	meshRender.transformMat = transformMat;
	unsigned int transID = RenderCommandList::setMeshTransform(transformMat);
	meshRender.instanceID = transID;
	for (int i = 0; i < mesh.meshParts.size(); i++) {
		RenderCommandList::setMeshPartTransform(&mesh.meshParts[i], meshRender.materials[i], transID);
		if (meshRender.enableOutline && meshRender.outlineMaterial != NULL)
			RenderCommandList::setMeshPartTransform(&mesh.meshParts[i], meshRender.outlineMaterial, transID);
	}
	for (int w = 0; w < wheelRenders.size(); w++) {
		transID = RenderCommandList::setMeshTransform(transformMat);
		wheelRenders[w]->instanceID = transID;
		for (int i = 0; i < wheelRenders[w]->mesh.meshParts.size(); i++) {
			RenderCommandList::setMeshPartTransform(&wheelRenders[w]->mesh.meshParts[i], wheelRenders[w]->materials[i], transID);
			if (wheelRenders[w]->enableOutline && wheelRenders[w]->outlineMaterial != NULL)
				RenderCommandList::setMeshPartTransform(&wheelRenders[w]->mesh.meshParts[i], wheelRenders[w]->outlineMaterial, transID);
		}
	}
}

Render * VehicleActor::getRender()
{
	return &meshRender;
}

unsigned int VehicleActor::getRenders(vector<Render*>& renders)
{
	renders.push_back(&meshRender);
	for (int i = 0; i < wheelRenders.size(); i++)
		renders.push_back(wheelRenders[i]);
	return 1 + wheelRenders.size();
}

void VehicleActor::setHidden(bool value)
{
}

bool VehicleActor::isHidden()
{
	return false;
}

void VehicleActor::setupPhysics(PhysicalWorld & physicalWorld)
{
	if (this->physicalWorld == NULL) {
		//Create the batched scene queries for the suspension raycasts.
		gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, 4, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, PhysicalWorld::gDefaultAllocatorCallback);
		gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, physicalWorld.physicsScene);

		physicalWorld.physicsScene->addActor(*rawVehicleActor);
		physicalWorld.onStepSimulation.addCallback(this, [](void* ptr, PhysicalWorld* world, float step) {
			((VehicleActor*)ptr)->update(step);
		});

		rawVehicleActor->setGlobalPose(getWorldTransform());
		this->physicalWorld = &physicalWorld;
	}
}

void VehicleActor::releasePhysics(PhysicalWorld & physicalWorld)
{
	if (this->physicalWorld != NULL) {
		gVehicle4W->getRigidDynamicActor()->release();
		gVehicle4W->free();
		gBatchQuery->release();
		gVehicleSceneQueryData->free(PhysicalWorld::gDefaultAllocatorCallback);
		frictionPairs->release();
		this->physicalWorld = NULL;
	}
}

void VehicleActor::PressGearUp()
{
	gVehicleInputData.setGearUp(true);
}

void VehicleActor::ReleaseGearUp()
{
	gVehicleInputData.setGearUp(false);
}

void VehicleActor::PressGearDown()
{
	gVehicleInputData.setGearDown(true);
}

void VehicleActor::ReleaseGearDown()
{
	gVehicleInputData.setGearDown(false);
}

void VehicleActor::PressAccelerator(bool back)
{
	if (back)
		if (gVehicle4W->mDriveDynData.getCurrentGear() != PxVehicleGearsData::eREVERSE)
		gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
	else if (gVehicle4W->mDriveDynData.getCurrentGear() < 2)
		gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void VehicleActor::ReleaseAccelerator()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(false);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(0.0f);
	}
}

void VehicleActor::PressBrake()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalBrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogBrake(1.0f);
	}
}

void VehicleActor::ReleaseBrake()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalBrake(false);
	}
	else
	{
		gVehicleInputData.setAnalogBrake(0.0f);
	}
}

void VehicleActor::PressHandBrake()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void VehicleActor::ReleaseHandBrake()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		gVehicleInputData.setAnalogHandbrake(0.0f);
	}
}

void VehicleActor::releaseAllControls()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(false);
		gVehicleInputData.setDigitalSteerLeft(false);
		gVehicleInputData.setDigitalSteerRight(false);
		gVehicleInputData.setDigitalBrake(false);
		gVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(0.0f);
		gVehicleInputData.setAnalogSteer(0.0f);
		gVehicleInputData.setAnalogBrake(0.0f);
		gVehicleInputData.setAnalogHandbrake(0.0f);
	}
}

void VehicleActor::SetSteering(float value)
{
	value = min(value, 1);
	value = max(value, -1);
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerLeft(value < 0);
		gVehicleInputData.setDigitalSteerRight(value > 0);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(value);
	}
}

void VehicleActor::setup()
{
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(4);
	PxVec3 wheelCenterActorOffsets[4];
	for (int i = 0; i < 4; i++)
		wheelCenterActorOffsets[i] = toPVec3(vehicleInfo.wheelInfos[i].wheelCenterActorOffset);
	PxF32 suspSprungMasses[4];
	PxVehicleComputeSprungMasses(4, wheelCenterActorOffsets,
		toPVec3(vehicleInfo.chassisCMOffset), vehicleInfo.chassisMass, 2, suspSprungMasses);
	for (int i = 0; i < 4; i++)
		setupWheels(*wheelsSimData, suspSprungMasses[i], vehicleInfo.wheelInfos[i]);

	PxVehicleDriveSimData4W driveSimData;
	setupDrive(driveSimData, *wheelsSimData);

	chassisPhysicsMat = PhysicalWorld::gPhysicsSDK->createMaterial(vehicleInfo.chassisFriction, vehicleInfo.chassisFriction, 0);
	wheelPhysicsMat = PhysicalWorld::gPhysicsSDK->createMaterial(vehicleInfo.wheelFriction, vehicleInfo.wheelFriction, 0);
	frictionPairs = createFrictionPairs(wheelPhysicsMat);

	rawVehicleActor = setupVehicleActor();

	gVehicle4W = PxVehicleDrive4W::allocate(4);
	gVehicle4W->setup(PhysicalWorld::gPhysicsSDK, rawVehicleActor, *wheelsSimData, driveSimData, 4);
	wheelsSimData->free();

	gVehicle4W->setToRestState();
	//gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	PressBrake();
}

void VehicleActor::setupWheels(PxVehicleWheelsSimData & wheelsSimData, float suspSprungMass, const WheelInfo& info)
{
	PxVehicleWheelData wheel;
	wheel.mMass = info.wheelMass;
	wheel.mRadius = info.wheelRadius;
	wheel.mWidth = info.wheelWidth;
	wheel.mMOI = 0.5 * info.wheelMass * info.wheelRadius * info.wheelRadius;
	wheel.mMaxHandBrakeTorque = info.maxHandBrakeTorque;
	wheel.mMaxSteer = info.maxSteer;
	wheel.mMaxBrakeTorque = info.maxBrakeTorque;
	PxVehicleTireData tire;
	tire.mType = TIRE_TYPE_NORMAL;
	PxVehicleSuspensionData suspension;
	suspension.mMaxCompression = info.suspensionCompression;
	suspension.mMaxDroop = info.suspensionDroop;
	suspension.mSpringStrength = info.springStrength;
	suspension.mSpringDamperRate = info.springDamperRate;
	suspension.mSprungMass = suspSprungMass;
	float sign = (info.type % 2) == 0 ? 1 : -1;
	suspension.mCamberAtRest = sign * info.camberAngleAtRest;
	suspension.mCamberAtMaxDroop = sign * info.camberAngleAtMaxDroop;
	suspension.mCamberAtMaxCompression = -sign * info.camberAngleAtMaxCompression;

	PxVec3 wheelCentreCMOffset = toPVec3(info.wheelCenterActorOffset - vehicleInfo.chassisCMOffset);
	PxVec3 suspForceAppCMOffset = PxVec3(wheelCentreCMOffset.x, wheelCentreCMOffset.y, -0.3f);
	PxVec3 tireForceAppCMOffset = PxVec3(wheelCentreCMOffset.x, wheelCentreCMOffset.y, -0.3f);

	PxFilterData qryFilterData;
	qryFilterData.word3 = UNDRIVABLE_SURFACE;

	wheelsSimData.setWheelData(info.type, wheel);
	wheelsSimData.setTireData(info.type, tire);
	wheelsSimData.setSuspensionData(info.type, suspension);
	wheelsSimData.setSuspTravelDirection(info.type, toPVec3(info.suspTravelDirection));
	wheelsSimData.setWheelCentreOffset(info.type, wheelCentreCMOffset);
	wheelsSimData.setSuspForceAppPointOffset(info.type, suspForceAppCMOffset);
	wheelsSimData.setTireForceAppPointOffset(info.type, tireForceAppCMOffset);
	wheelsSimData.setSceneQueryFilterData(info.type, qryFilterData);
	wheelsSimData.setWheelShapeMapping(info.type, info.type);
}

void VehicleActor::setupDrive(PxVehicleDriveSimData4W & driveSimData, const PxVehicleWheelsSimData & wheelsSimData)
{
	//Diff
	PxVehicleDifferential4WData diff;
	diff.mType = vehicleInfo.type;
	driveSimData.setDiffData(diff);

	//Engine
	PxVehicleEngineData engine;
	engine.mMOI = 2.2;
	engine.mPeakTorque = vehicleInfo.peakTorque;
	engine.mMaxOmega = vehicleInfo.maxOmega;
	driveSimData.setEngineData(engine);

	//Gears
	PxVehicleGearsData gears;
	gears.mSwitchTime = vehicleInfo.gearSwitchTime;
	driveSimData.setGearsData(gears);

	//Clutch
	PxVehicleClutchData clutch;
	clutch.mStrength = vehicleInfo.clutchStrength;
	driveSimData.setClutchData(clutch);

	//AutoBox
	PxVehicleAutoBoxData autoBox;
	autoBox.setLatency(vehicleInfo.gearSwitchTime + 1);
	driveSimData.setAutoBoxData(autoBox);

	//Ackermann steer accuracy
	PxVehicleAckermannGeometryData ackermann;
	ackermann.mAccuracy = 1.0f;
	ackermann.mAxleSeparation = abs(
		wheelsSimData.getWheelCentreOffset(FRONT_LEFT).x -
		wheelsSimData.getWheelCentreOffset(REAR_LEFT).x);
	ackermann.mFrontWidth = abs(
		wheelsSimData.getWheelCentreOffset(FRONT_RIGHT).y -
		wheelsSimData.getWheelCentreOffset(FRONT_LEFT).y);
	ackermann.mRearWidth = abs(
		wheelsSimData.getWheelCentreOffset(REAR_RIGHT).y -
		wheelsSimData.getWheelCentreOffset(REAR_LEFT).y);
	driveSimData.setAckermannGeometryData(ackermann);
}

PxRigidDynamic * VehicleActor::setupVehicleActor()
{
	PxRigidDynamic* vehActor = PhysicalWorld::gPhysicsSDK->createRigidDynamic(PxTransform(PxIdentity));

	//Wheel and chassis query filter data.
	//Optional: cars don't drive on other cars.
	PxFilterData wheelQryFilterData;
	wheelQryFilterData.word3 = UNDRIVABLE_SURFACE;
	PxFilterData chassisQryFilterData;
	chassisQryFilterData.word3 = UNDRIVABLE_SURFACE;

	PxFilterData wheelSimFilterData;
	wheelSimFilterData.word0 = COLLISION_FLAG_WHEEL;
	wheelSimFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;
	wheelSimFilterData.word2 = PxPairFlag::eMODIFY_CONTACTS;

	PxFilterData chassisSimFilterData;
	chassisSimFilterData.word0 = COLLISION_FLAG_CHASSIS;
	chassisSimFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;

	//Add all the wheel shapes to the actor.
	for (PxU32 i = 0; i < 4; i++)
	{
		PxShape* wheelShape = PxRigidActorExt::createExclusiveShape(*vehActor, *vehicleInfo.wheelInfos[i].mesh->generateComplexCollisionShape(), *wheelPhysicsMat);
		wheelShape->setQueryFilterData(wheelQryFilterData);
		wheelShape->setSimulationFilterData(wheelSimFilterData);
		wheelShape->setLocalPose(PxTransform(PxIdentity));
	}

	//Add the chassis shapes to the actor.
	PxShape* chassisShape = PxRigidActorExt::createExclusiveShape(*vehActor,
		chassisCollisioncComplexType == SIMPLE ? *chassisCollision->generateCollisionShape()
		: *chassisCollision->generateComplexCollisionShape(), *chassisPhysicsMat);
	if (chassisCollisioncComplexType == SIMPLE)
		chassisShape->setLocalPose(toPTransform(chassisCollision->getCenter()));
	chassisShape->setQueryFilterData(chassisQryFilterData);
	chassisShape->setSimulationFilterData(chassisSimFilterData);
	chassisShape->setLocalPose(PxTransform(PxIdentity));

	vehActor->setMass(vehicleInfo.chassisMass);

	PxVec3 chassisDims(chassisCollision->getDepth(), chassisCollision->getWidth(), chassisCollision->getHeight());
	PxVec3 chassisMOI
	((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*vehicleInfo.chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*vehicleInfo.chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*0.8f*vehicleInfo.chassisMass / 12.0f);

	vehActor->setMassSpaceInertiaTensor(chassisMOI);
	vehActor->setCMassLocalPose(PxTransform(toPVec3(vehicleInfo.chassisCMOffset), PxQuat(PxIdentity)));

	return vehActor;
}

void VehicleActor::update(float timestep)
{
	//Update the control inputs for the vehicle.
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	PxVec3 grav = physicalWorld->physicsScene->getGravity();
	PxWheelQueryResult wheelQueryResults[4];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels() } };
	PxVehicleUpdates(timestep, grav, *frictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	apply(rawVehicleActor->getGlobalPose());
	Matrix4f wt = getMatrix(WORLD);

	for (int i = 0; i < vehicleQueryResults->nbWheelQueryResults; i++) {
		PTransform& pTran = vehicleQueryResults->wheelQueryResults[i].localPose;
		Matrix4f T = Matrix4f::Identity();
		T(0, 3) = pTran.p.x;
		T(1, 3) = pTran.p.y;
		T(2, 3) = pTran.p.z;
		Matrix4f R = Matrix4f::Identity();
		R.block(0, 0, 3, 3) = toQuaternionf(pTran.q).toRotationMatrix();
		wheelRenders[i]->transformMat = wt * (T * R);
		wheelStates[i].tireLongitudinalDir = toVector3f(vehicleQueryResults->wheelQueryResults[i].tireLongitudinalDir);
		wheelStates[i].tireFriction = vehicleQueryResults->wheelQueryResults[i].tireFriction;
		wheelStates[i].suspSpringForce = vehicleQueryResults->wheelQueryResults[i].suspSpringForce;
		wheelStates[i].longitudinalSlip = vehicleQueryResults->wheelQueryResults[i].longitudinalSlip;
		wheelStates[i].steerAngle = vehicleQueryResults->wheelQueryResults[i].steerAngle;
	}
	vehicleState.gear = gVehicle4W->mDriveDynData.getCurrentGear();
	vehicleState.engineRotationSpeed = gVehicle4W->mDriveDynData.getEngineRotationSpeed();
	vehicleState.forwardSpeed = gVehicle4W->computeForwardSpeed();
	vehicleState.sidewaySpeed = gVehicle4W->computeSidewaysSpeed();
}
