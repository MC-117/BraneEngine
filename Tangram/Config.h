#pragma once
#define ENGINE_VERSION "0.1.0"
#define PI (3.1415926535897932346f)

#define VENDOR_USE_OPENGL
#define VENDOR_USE_DX12

#define PHYSICS_USE_PHYSX
//#define PHYSICS_USE_BULLET

#include "sTOB.h"
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <list>
#include <set>
#include <unordered_set>
#include <thread>
#include <sys/timeb.h>
#include "imgui.h"
#include "ImGuizmo.h"
#include <GL\glew.h>
//#include <GL\freeglut.h>
#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>
#include <Dense>
#ifdef PHYSICS_USE_BULLET
#include <btBulletDynamicsCommon.h>
typedef btCollisionObject CollisionObject;
typedef btCollisionShape CollisionShape;
typedef btRigidBody CollisionRigidBody;
typedef btVector3 PVec3;
typedef btQuaternion PQuat;
typedef btTransform PTransform;
#endif // PHYSICS_USE_BULLET

#ifdef PHYSICS_USE_PHYSX
#include <PxPhysicsApi.h>
using namespace physx;
typedef PxActor CollisionObject;
typedef PxGeometry CollisionShape;
typedef PxRigidBody CollisionRigidBody;
typedef PxVec3 PVec3;
typedef PxQuat PQuat;
typedef PxTransform PTransform;

#include <NvCloth\Factory.h>
#include <NvCloth\Cloth.h>
#include <NvCloth\Fabric.h>
#include <NvCloth\Solver.h>
#include <JobManager.h>

typedef nv::cloth::Fabric PFabric;
typedef nv::cloth::Cloth PCloth;
#endif // PHYSICS_USE_PHYSX

#include <AL\alut.h>
#include "Delegate.h"
#include "Brane.h"

#define VERTEX_MAX_BONE 4

#define TRANS_BIND_INDEX 0
#define MORPHDATA_BIND_INDEX 1
#define MORPHWEIGHT_BIND_INDEX 2
#define PARTICLE_BIND_INDEX 3
#define MODIFIER_BIND_INDEX 4
#define TRANS_INDEX_BIND_INDEX 5
#define LIGHT_BIND_INDEX 6
#define CAM_BIND_INDEX 7