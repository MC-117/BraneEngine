#pragma once
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Events.h"
#include "Render.h"
#include "Serialization.h"
#include "PythonScript.h"

class PhysicalWorld;

class Object : public Serializable {
public:
	Serialize(Object)

	string name;
	Object* parent = NULL;
	vector<Object*> children;
	Events events;
	PythonRuntimeObject pythonRuntimeObject;

	Object(string name = "Object");
	virtual ~Object();

	virtual void begin();
	virtual void tick(float deltaTime);
	virtual void afterTick();
	virtual void prerender();
	virtual void end();

	virtual void onAttacted(Object& parent);
	virtual void onDistacted(Object& parent);

	virtual Render* getRender();
	virtual unsigned int getRenders(vector<Render*>& renders);
	virtual void* getPhysicalBody();
	virtual void setupPhysics(PhysicalWorld& physicalWorld);
	virtual void releasePhysics(PhysicalWorld& physicalWorld);

	void bindBegin(void(*beginHandler)(Object* sender));
	void bindTick(void(*tickHandler)(Object* sender, float deltaTime));
	void bindAfterTick(void(*afterTickHandler)(Object* sender));
	void bindEnd(void(*endHandler)(Object* sender));

	void unbindBegin(void(*beginHandler)(Object* sender));
	void unbindTick(void(*tickHandler)(Object* sender, float deltaTime));
	void unbindAfterTick(void(*afterTickHandler)(Object* sender));
	void unbindEnd(void(*endHandler)(Object* sender));

	Object* getRoot() const;
	Object* getSibling() const;
	Object* getChild() const;
	Object* findChild(const string& name);
	virtual void setParent(Object& parent);
	virtual void unparent();
	virtual void addChild(Object& child);
	void clearChild();

	void traverse(void(*preFunc)(Object& object), void(*postFunc)(Object& object));
	void traverse(bool(*preFunc)(Object& object), void(*postFunc)(Object& object, bool));
	void preTraverse(void(*func)(Object& object));
	void preTraverse(bool(*func)(Object& object));
	void preTraverse(bool(*func)(Object& object, float), float arg);
	void postTraverse(void(*func)(Object& object));
	virtual void destroy(bool applyToChild = false);
	bool isDestroy();
	bool isinitialized();
	bool isInternal();

	static Serializable* instantiate(const SerializationInfo& from);
	virtual bool deserialize(const SerializationInfo& from);
	virtual bool serialize(SerializationInfo& to);
protected:
	bool initialized = false;
	bool tryDestroy = false;
	bool internalNode = false; // internal node: can not change its hierarchy
	int siblingIdx = -1;
	
	Delegate<void(Object*)> beginHandler;
	Delegate<void(Object*, float)> tickHandler;
	Delegate<void(Object*)> afterTickHandler;
	Delegate<void(Object*)> endHandler;

	virtual void addInternalNode(Object& object);
};

class ObjectIterator
{
public:
	ObjectIterator(Object* root);

	bool next();
	Object& current();
	Object* unparentCurrent();
	void reset(Object* root = NULL);
protected:
	Object *root = NULL, *curObj = NULL;
	bool delay = false;
};

enum InstanceRuleEnum
{
	IR_Default, IR_WorldUniqueName, IR_ExistUniqueName
};

void ChildrenInstantiateObject(const SerializationInfo& from, Object* pObj, InstanceRuleEnum rule = IR_Default);

#define ChildrenInstantiate(Type, from, pObj) ChildrenInstantiate##Type(from, pObj);

#endif // !_OBJECT_H_
