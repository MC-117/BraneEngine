#pragma once
#ifndef _PHYSICALWORLD_H_
#define _PHYSICALWORLD_H_

#include "RigidBody.h"
#include "PhysicalController.h"

// Helper class; draws the world as seen by Bullet.
// This is very handy to see it Bullet's world matches yours.
// This example uses the old OpenGL API for simplicity, 
// so you'll have to remplace GLFW_OPENGL_CORE_PROFILE by
// GLFW_OPENGL_COMPAT_PROFILE in glfwWindowHint()
// How to use this class :
// Declare an instance of the class :
// BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;
// physicsScene->setDebugDrawer(&mydebugdrawer);
// Each frame, call it :
// mydebugdrawer.SetMatrices(ViewMatrix, ProjectionMatrix);
// physicsScene->debugDrawWorld();

#ifdef PHYSICS_USE_BULLET
class BulletDebugDrawer_DeprecatedOpenGL : public btIDebugDraw{
public:
	void SetMatrices(Matrix4f pViewMatrix, Matrix4f pProjectionMatrix){
		glUseProgram(0); // Use Fixed-function pipeline (no shaders)
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(pViewMatrix.data());
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(pProjectionMatrix.data());
	}
	virtual void drawLine(const PVec3& from,const PVec3& to,const PVec3& color){
		glColor3f(color.x(), color.y(), color.z());
		glBegin(GL_LINES);
			glVertex3f(from.x(), from.y(), from.z());
			glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}
	virtual void drawContactPoint(const PVec3 &,const PVec3 &,btScalar,int,const PVec3 &){}
	virtual void reportErrorWarning(const char *){}
	virtual void draw3dText(const PVec3 &,const char *){}
	virtual void setDebugMode(int p){
		m = p;
	}
	int getDebugMode(void) const {return 3;}
	int m;
};
#endif

#ifdef PHYSICS_USE_PHYSX
#endif // PHYSICS_USE_PHYSX
class PhysicalWorld;
struct StepSimulationCallBack
{
	map<void*, void(*)(void* ptr, PhysicalWorld*, float)> callbacks;

	void addCallback(void* target, void(*callback)(void* ptr, PhysicalWorld*, float)) {
		callbacks[target] = callback;
	}

	void removeCallback(void* target) {
		callbacks.erase(target);
	}

	void operator()(PhysicalWorld* world, float timestep) {
		for (auto b = callbacks.begin(), e = callbacks.end(); b != e; b++)
			if (b->second != NULL)
				b->second(b->first, world, timestep);
	}
};

class PxBEErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		string codeStr;
		if (code == PxErrorCode::eNO_ERROR) {
			return;
		}
		if (code == PxErrorCode::eMASK_ALL) {
			codeStr = "eMASK_ALL";
		}
		//! \brief An informational message.
		if (code & PxErrorCode::eDEBUG_INFO)
			codeStr = "|eDEBUG_INFO";
		//! \brief a warning message for the user to help with debugging
		if (code & PxErrorCode::eDEBUG_WARNING)
			codeStr = "|eDEBUG_WARNING";

		//! \brief method called with invalid parameter(s)
		if (code & PxErrorCode::eINVALID_PARAMETER)
			codeStr = "|eINVALID_PARAMETER";

		//! \brief method was called at a time when an operation is not possible
		if (code & PxErrorCode::eINVALID_OPERATION)
			codeStr = "|eINVALID_OPERATION";

		//! \brief method failed to allocate some memory
		if (code & PxErrorCode::eOUT_OF_MEMORY)
			codeStr = "|eOUT_OF_MEMORY";

		/** \brief The library failed for some reason.
		Possibly you have passed invalid values like NaNs, which are not checked for.
		*/
		if (code & PxErrorCode::eINTERNAL_ERROR)
			codeStr = "|eINTERNAL_ERROR";

		//! \brief An unrecoverable error, execution should be halted and log output flushed
		if (code & PxErrorCode::eABORT)
			codeStr = "|eABORT";

		//! \brief The SDK has determined that an operation may result in poor performance.
		if (code & PxErrorCode::ePERF_WARNING)
			codeStr = "|ePERF_WARNING";
		Console::error("PxError(%s): %s, File(%s), Line(%d)", codeStr.c_str(), message, file, line);
	}
};

class PhysicalWorld
#ifdef PHYSICS_USE_PHYSX
	: public PxContactModifyCallback
#endif // PHYSICS_USE_PHYSX
{
public:
#ifdef PHYSICS_USE_BULLET
	btDefaultCollisionConfiguration collisionConfiguration;
	btCollisionDispatcher dispatcher;
	btDbvtBroadphase overlappingPairCache;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld* physicsScene = NULL;
	BulletDebugDrawer_DeprecatedOpenGL db;

	void collisionDispatch();
#endif // PHYSICS_USE_BULLET

#ifdef PHYSICS_USE_PHYSX
	static PxPhysics* gPhysicsSDK;
	static PxBEErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	static PxSimulationFilterShader gDefaultFilterShader;
	static PxFoundation* gFoundation;
	static PxPvd* gPvd;
	static PxCooking* gCooking;
	static PxDefaultCpuDispatcher* gDispatcher;
	static PxCudaContextManager* gCudaContextManager;
	static nv::cloth::Factory* gNvClothFactory;

	PxScene* physicsScene = NULL;
	PxControllerManager* controllerManager = NULL;
	nv::cloth::Solver* clothSolver = NULL;

	float accumulator = 0.0f;
	float stepSize = 1.0f / 60.0f;

	StepSimulationCallBack onStepSimulation;

	virtual void onContactModify(PxContactModifyPair* const pairs, PxU32 count);
	static void drawDedug(ImDrawList* list, const PxRenderBuffer& buffer, Camera& cam);
#endif // PHYSICS_USE_PHYSX

	PhysicalWorld();
	virtual ~PhysicalWorld();

	void addPhysicalBody(PhysicalBody& body);
	void removePhysicalBody(PhysicalBody& body);

	Vector3f getGravity();
	void setGravity(const Vector3f& g);

	void updatePhysicalWorld(float deltaTime);

	bool rayTest(const Vector3f& startPoint, const Vector3f& endPoint, ContactInfo& result);
	bool rayTestAll(const Vector3f& startPoint, const Vector3f& endPoint, vector<ContactInfo>& result);

	static bool init();
	static void release();
protected:
	static bool isInit;
};

#endif // !_PHYSICALWORLD_H_
