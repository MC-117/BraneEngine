#pragma once
#ifndef _VEHICLEACTOR_H_
#define _VEHICLEACTOR_H_

#include "MeshActor.h"

enum DRIVABLE
{
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};

enum VEHICLE_COLLISION_FLAG
{
	COLLISION_FLAG_GROUND = 1 << 0,
	COLLISION_FLAG_WHEEL = 1 << 1,
	COLLISION_FLAG_CHASSIS = 1 << 2,
	COLLISION_FLAG_OBSTACLE = 1 << 3,
	COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

	COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
};

enum SURFACE_TYPE
{
	SURFACE_TYPE_TARMAC,
	MAX_NUM_SURFACE_TYPES
};

//Tire types.
enum TIRE_TYPE
{
	TIRE_TYPE_NORMAL = 0,
	//TIRE_TYPE_WORN,
	MAX_NUM_TIRE_TYPES
};

class VehicleActor;

enum WheelType
{
	FRONT_LEFT, FRONT_RIGHT, REAR_LEFT, REAR_RIGHT
};

class VehicleSceneQueryData
{
public:
	VehicleSceneQueryData();
	~VehicleSceneQueryData();

	//Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
	static VehicleSceneQueryData* allocate
	(const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, const PxU32 maxNumHitPointsPerWheel, const PxU32 numVehiclesInBatch,
		PxBatchQueryPreFilterShader preFilterShader, PxBatchQueryPostFilterShader postFilterShader,
		PxAllocatorCallback& allocator);

	//Free allocated buffers.
	void free(PxAllocatorCallback& allocator);

	//Create a PxBatchQuery instance that will be used for a single specified batch.
	static PxBatchQuery* setUpBatchedSceneQuery(const PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene);

	//Return an array of scene query results for a single specified batch.
	PxRaycastQueryResult* getRaycastQueryResultBuffer(const PxU32 batchId);

	//Return an array of scene query results for a single specified batch.
	PxSweepQueryResult* getSweepQueryResultBuffer(const PxU32 batchId);

	//Get the number of scene query results that have been allocated for a single batch.
	PxU32 getQueryResultBufferSize() const;

private:

	//Number of queries per batch
	PxU32 mNumQueriesPerBatch;

	//Number of hit results per query
	PxU32 mNumHitResultsPerQuery;

	//One result for each wheel.
	PxRaycastQueryResult* mRaycastResults;
	PxSweepQueryResult* mSweepResults;

	//One hit for each wheel.
	PxRaycastHit* mRaycastHitBuffer;
	PxSweepHit* mSweepHitBuffer;

	//Filter shader used to filter drivable and non-drivable surfaces
	PxBatchQueryPreFilterShader mPreFilterShader;

	//Filter shader used to reject hit shapes that initially overlap sweeps.
	PxBatchQueryPostFilterShader mPostFilterShader;

};

struct WheelInfo
{
	Mesh* mesh = NULL;
	Material* material = NULL;
	WheelType type;
	//suspensions
	float suspensionDroop = 0.1f;
	float suspensionCompression = 0.3f;
	float springDamperRate = 4500.0f;
	float springStrength = 35000.0f;

	float camberAngleAtRest = 0.0f;
	float camberAngleAtMaxDroop = 0.01f;
	float camberAngleAtMaxCompression = 0.01f;

	float maxHandBrakeTorque = 4000;
	float maxBrakeTorque = 4000;
	float maxSteer = PI / 3;

	float wheelRadius;
	float wheelWidth;
	float wheelMass = 20.0f;

	Vector3f wheelCenterActorOffset = { 0, 0, 0 };
	Vector3f suspTravelDirection = { 0, 0, -1 };
};
//
//class WheelActor : public Actor
//{
//public:
//	VehicleActor* owner = NULL;
//	MeshRender meshRender;
//	WheelInfo wheelInfo;
//
//	WheelActor(Mesh& mesh, Material& material, WheelInfo& wheelInfo, string path = "WheelActor");
//
//	virtual void prerender();
//	virtual Render* getRender();
//	virtual unsigned int getRenders(vector<Render*>& renders);
//
//	virtual void setHidden(bool value);
//	virtual bool isHidden();
//
//protected:
//	Mesh& mesh;
//};
//
//class btBetterVehicleRaycaster : public btVehicleRaycaster
//{
//	btDynamicsWorld* m_dynamicsWorld = NULL;
//
//public:
//	void setPhysicalWorld(PhysicalWorld& physicalWorld);
//
//	virtual void* castRay(const PVec3& from, const PVec3& to, btVehicleRaycasterResult& result);
//};
//
struct VehicleInfo
{
	PxVehicleDifferential4WData::Enum type = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	WheelInfo wheelInfos[4];

	//Engine
	float peakTorque = 500.0f;
	float maxOmega = 600.0f;

	//Gear
	float gearSwitchTime = 0.5f;

	float clutchStrength = 10.0f;
	float chassisMass = 1500.0f;

	float chassisFriction = 0;
	float wheelFriction = 1;

	Vector3f chassisCMOffset = { 0, 0, 0 };
};

struct WheelState
{
	float tireFriction = 0;
	float suspSpringForce = 0;
	float steerAngle = 0;
	float longitudinalSlip = 0;
	Vector3f tireLongitudinalDir;
};

struct VehicleState
{
	unsigned int gear = 0;
	float engineRotationSpeed = 0;
	float forwardSpeed = 0;
	float sidewaySpeed = 0;
};

class VehicleActor : public Actor
{
public:
	MeshRender meshRender;
	VehicleState vehicleState;
	WheelState wheelStates[4];
	vector<MeshRender*> wheelRenders;
	VehicleActor(VehicleInfo& vehicleInfo, Mesh& mesh, Material& material, Shape* chassisCollision, ShapeComplexType complexType = SIMPLE, string name = "VehicleActor");
	virtual ~VehicleActor();

	virtual void tick(float deltaTime);
	
	virtual void prerender();
	virtual Render* getRender();
	virtual unsigned int getRenders(vector<Render*>& renders);

	virtual void setHidden(bool value);
	virtual bool isHidden();

	virtual void setupPhysics(PhysicalWorld& physicalWorld);
	virtual void releasePhysics(PhysicalWorld& physicalWorld);

	void PressGearUp();
	void ReleaseGearUp();
	void PressGearDown();
	void ReleaseGearDown();
	void PressAccelerator(bool back);
	void ReleaseAccelerator();
	void PressBrake();
	void ReleaseBrake();
	void PressHandBrake();
	void ReleaseHandBrake();
	void SetSteering(float value);
	void releaseAllControls();
	void update(float timestep);
protected:
	Mesh &mesh;
	Shape* chassisCollision = NULL;
	ShapeComplexType chassisCollisioncComplexType = SIMPLE;
	VehicleInfo vehicleInfo;
	PxRigidDynamic* rawVehicleActor = NULL;
	PxVehicleDrive4W* gVehicle4W = NULL;
	PxVehicleDrivableSurfaceToTireFrictionPairs* frictionPairs = NULL;
	VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;
	PxMaterial* chassisPhysicsMat = NULL;
	PxMaterial* wheelPhysicsMat = NULL;

	PhysicalWorld* physicalWorld = NULL;

	bool gIsVehicleInAir = true;

	PxF32 gSteerVsForwardSpeedData[16] =
	{
		0.0f,		0.75f,
		5.0f,		0.75f,
		30.0f,		0.125f,
		120.0f,		0.1f,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32
	};
	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable = PxFixedSizeLookupTable<8>(gSteerVsForwardSpeedData, 4);

	PxVehicleKeySmoothingData gKeySmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehiclePadSmoothingData gPadSmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehicleDrive4WRawInputData gVehicleInputData;
	bool					gMimicKeyInputs = false;

	void setup();
	void setupWheels(PxVehicleWheelsSimData& wheelsSimData, float suspSprungMass, const WheelInfo& info);
	void setupDrive(PxVehicleDriveSimData4W& driveSimData, const PxVehicleWheelsSimData& wheelsSimData);
	PxRigidDynamic* setupVehicleActor();
};

#endif // !_VEHICLEACTOR_H_
