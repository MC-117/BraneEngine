#pragma once
#ifndef _PYTHONMANAGER_H_
#define _PYTHONMANAGER_H_

#include "Unit.h"
#include <Python.h>
#include <structmember.h>

class PythonManager
{
private:
	static bool isInit;
	static PyObject* pyMainMod;
public:
	static vector<PyMethodDef> methods;
	static vector<pair<const char*, PyTypeObject*>> typeObjects;
	static map<PyTypeObject*, set<PyObject*>> runtimeObjects;
	static map<const char*, long> constInt;
	static PyModuleDef engineMod;
	static void start();
	static void run(const string& code);
	static void end();

	static PyObject* getMainModule();
	static void regist(PyObject* obj);
	static void logoff(PyObject* obj);
};

class UtilityPy
{
public:
	static PyObject* getWorld(PyObject *self, PyObject *args);
	static PyObject* getInput(PyObject *self, PyObject *args);
	static PyObject* destroyObject(PyObject *self, PyObject *args);
	static PyObject* getAllAssetName(PyObject *self, PyObject *args);
	static PyObject* spawnMeshActor(PyObject *self, PyObject *args);
	static PyObject* setGravity(PyObject *self, PyObject *args);
	static PyObject* msgBox(PyObject *self, PyObject *args);

	static UtilityPy instance;
	UtilityPy();
};

class ConsolePy
{
public:
	static PyModuleDef logMod;
	static PyMethodDef logMethods[3];
	static PyModuleDef errorMod;
	static PyMethodDef errorMethods[3];
	static ConsolePy instance;
	static PyObject* log(PyObject *self, PyObject *args);
	static PyObject* warn(PyObject *self, PyObject *args);
	static PyObject* error(PyObject *self, PyObject *args);
	static PyObject* pyLogWrite(PyObject *self, PyObject *args);
	static PyObject* pyLogFlush(PyObject *self, PyObject *args);
	static PyObject* pyErrWrite(PyObject *self, PyObject *args);
	static PyObject* pyErrFlush(PyObject *self, PyObject *args);

	ConsolePy();
};

class Vec3Py
{
public:
	struct Vec3
	{
		PyObject_HEAD;
		float x, y, z;
	};
	static PyNumberMethods PyVec3_NumMethods;
	static PyMemberDef PyVec3_Members[4];
	static PyTypeObject PyVec3_Type;
	static Vec3Py instance;

	static void Vec3_dealloc(Vec3* self);
	static PyObject* Vec3_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
	static int Vec3_init(Vec3 *self, PyObject *args, PyObject *kwds);
	static PyObject* Vec3_str(Vec3 *obj);
	static Vec3* Vec3_add(Vec3* a, Vec3* b);
	static Vec3* Vec3_sub(Vec3* a, Vec3* b);

	Vec3Py();

	static PyObject* New(float x = 0, float y = 0, float z = 0);
	static PyObject* New(const Vector3f& v);

	static Vec3* cast(PyObject* pobj);
};

class ShapePy
{
public:
	struct _Shape
	{
		PyObject_HEAD;
		Vec3Py::Vec3* minPos;
		Vec3Py::Vec3* maxPos;
		PyObject* type;
	};
	static PyMemberDef PyShape_Members[4];
	static PyTypeObject PyShape_Type;
	static ShapePy instance;

	static void Shape_dealloc(_Shape* self);
	static PyObject* Shape_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
	static int Shape_init(_Shape *self, PyObject *args, PyObject *kwds);
	static PyObject* Shape_str(_Shape *obj);

	ShapePy();
};

struct PyCPointer
{
	PyObject_HEAD;
	void* nativeHandle;
};

class MeshPy
{
public:
	static PyMemberDef PyMesh_Members[4];
	static PyTypeObject PyMesh_Type;
	static MeshPy instance;

	static void Mesh_dealloc(PyCPointer* self);
	static PyObject* Mesh_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
	static int Mesh_init(PyCPointer *self, PyObject *args, PyObject *kwds);
	static PyObject* Mesh_str(PyCPointer *obj);
};

class ObjectPy
{
public:
	static PyMemberDef Members[2];
	static PyMethodDef Methods[3];
	static PyTypeObject Type;
	static ObjectPy instance;

	static void __dealloc__(PyObject* self);
	static PyObject* __new__(PyTypeObject *type, PyObject *args, PyObject *kwds);
	static int __init__(PyCPointer *self, PyObject *args, PyObject *kwds);
	static PyObject* __str__(PyCPointer *obj);

	static PyObject * getName(PyObject * self, PyObject * args);
	static PyObject * destroy(PyObject * self, PyObject * args);

	template<class T>
	static T* cast(PyObject* pyCPtr)
	{
		return dynamic_cast<T*>((Object*)((PyCPointer*)pyCPtr)->nativeHandle);
	}

	static PyObject* New(PyTypeObject* type, void* ptr);

	ObjectPy();
};

class InputPy
{
public:
	static PyMethodDef Methods[14];
	static PyTypeObject Type;
	static InputPy instance;

	static PyObject* __str__(PyCPointer *obj);

	static PyObject* getKeyUp(PyObject * self, PyObject * args);
	static PyObject* getKeyDown(PyObject * self, PyObject * args);
	static PyObject* getKeyPress(PyObject * self, PyObject * args);
	static PyObject* getKeyRelease(PyObject * self, PyObject * args);
	static PyObject* getMousePos(PyObject * self, PyObject * args);
	static PyObject* getCursorPos(PyObject * self, PyObject * args);
	static PyObject* getMouseMove(PyObject * self, PyObject * args);
	static PyObject* getCursorMove(PyObject * self, PyObject * args);
	static PyObject* getMouseWheelValue(PyObject * self, PyObject * args);
	static PyObject* getMouseButtonUp(PyObject * self, PyObject * args);
	static PyObject* getMouseButtonDown(PyObject * self, PyObject * args);
	static PyObject* getMouseButtonPress(PyObject * self, PyObject * args);
	static PyObject* getMouseButtonRelease(PyObject * self, PyObject * args);

	InputPy();
};

class TransformPy
{
public:
	static PyMethodDef Methods[10];
	static PyTypeObject Type;
	static TransformPy instance;

	static PyObject* __str__(PyCPointer *obj);

	static PyObject* getPosition(PyObject * self, PyObject * args);
	static PyObject* getEulerAngle(PyObject * self, PyObject * args);
	static PyObject* getScale(PyObject * self, PyObject * args);
	static PyObject* setPosition(PyObject * self, PyObject * args);
	static PyObject* setRotation(PyObject * self, PyObject * args);
	static PyObject* setScale(PyObject * self, PyObject * args);
	static PyObject* translate(PyObject * self, PyObject * args);
	static PyObject* rotate(PyObject * self, PyObject * args);
	static PyObject* scale(PyObject * self, PyObject * args);

	TransformPy();
};

class ActorPy
{
public:
	static PyMethodDef Methods[6];
	static PyTypeObject Type;
	static ActorPy instance;

	static PyObject* __str__(PyCPointer *obj);

	static PyObject * isHidden(PyObject * self, PyObject * args);
	static PyObject * setHidden(PyObject * self, PyObject * args);

	static PyObject * playAudio(PyObject * self, PyObject * args);
	static PyObject * pauseAudio(PyObject * self, PyObject * args);
	static PyObject * stopAudio(PyObject * self, PyObject * args);

	ActorPy();
};

class SkeletonMeshActorPy
{
public:
	static PyMethodDef Methods[4];
	static PyTypeObject Type;
	static SkeletonMeshActorPy instance;

	static PyObject* __str__(PyCPointer *obj);

	static PyObject * playAnimation(PyObject * self, PyObject * args);
	static PyObject * pauseAnimation(PyObject * self, PyObject * args);
	static PyObject * stopAnimation(PyObject * self, PyObject * args);

	SkeletonMeshActorPy();
};

#endif // !_PYTHONMANAGER_H_
