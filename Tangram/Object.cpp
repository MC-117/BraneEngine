#include "Object.h"

SerializeInstance(Object);

Object::Object(string name) : events(this), pythonRuntimeObject(this)
{
	this->name = name;
	Brane::born(typeid(Object).hash_code(), this, name);
}

Object::~Object()
{
	end();
}

void Object::begin()
{
	beginHandler(this);
	pythonRuntimeObject.begin();
	initialized = true;
}

void Object::tick(float deltaTime)
{
	tickHandler(this, deltaTime);
	pythonRuntimeObject.tick(deltaTime);
}

void Object::afterTick()
{
	afterTickHandler(this);
	pythonRuntimeObject.afterTick();
}

void Object::prerender()
{
}

void Object::end()
{
	endHandler(this);
	pythonRuntimeObject.end();
	initialized = false;
	unparent();
	clearChild();
	Brane::vanish(typeid(Object).hash_code(), this);
}

void Object::onAttacted(Object & parent)
{
}

void Object::onDistacted(Object & parent)
{
}

Render* Object::getRender()
{
	return NULL;
}

unsigned int Object::getRenders(vector<Render*>& renders)
{
	return 0;
}

void * Object::getPhysicalBody()
{
	return nullptr;
}

void Object::setupPhysics(PhysicalWorld & physicalWorld)
{
}

void Object::releasePhysics(PhysicalWorld & physicalWorld)
{
}

void Object::bindBegin(void(*beginHandler)(Object* sender))
{
	this->beginHandler += beginHandler;
}

void Object::bindTick(void(*tickHandler)(Object* sender, float deltaTime))
{
	this->tickHandler += tickHandler;
}

void Object::bindAfterTick(void(*afterTickHandler)(Object* sender))
{
	this->afterTickHandler += afterTickHandler;
}

void Object::bindEnd(void(*endHandler)(Object* sender))
{
	this->endHandler += endHandler;
}

void Object::unbindBegin(void(*beginHandler)(Object *sender))
{
	this->beginHandler -= beginHandler;
}

void Object::unbindTick(void(*tickHandler)(Object *sender, float deltaTime))
{
	this->tickHandler -= tickHandler;
}

void Object::unbindAfterTick(void(*afterTickHandler)(Object *sender))
{
	this->afterTickHandler -= afterTickHandler;
}

void Object::unbindEnd(void(*endHandler)(Object *sender))
{
	this->endHandler -= endHandler;
}

Object * Object::getRoot() const
{
	if (parent == NULL)
		return NULL;
	Object* obj = parent;
	while (obj->parent != NULL)
		obj = obj->parent;
	return obj;
}

Object * Object::getSibling() const
{
	if (parent == NULL)
		return NULL;
	int i = siblingIdx + 1;
	if (i < parent->children.size())
		return parent->children[i];
	return NULL;
}

Object * Object::getChild() const
{
	if (children.size() != 0)
		return children[0];
	return NULL;
}

Object * Object::findChild(const string & name)
{
	if (this->name == name)
		return this;
	for (int i = 0; i < children.size(); i++) {
		Object* obj = children[i]->findChild(name);
		if (obj != NULL)
			return obj;
	}
	return NULL;
}

void Object::setParent(Object & parent)
{
	unparent();
	this->parent = &parent;
	siblingIdx = parent.children.size();
	parent.children.push_back(this);
	onAttacted(parent);
}

void Object::unparent()
{
	if (parent != NULL) {
		if (siblingIdx != -1) {
			auto b = parent->children.erase(parent->children.begin() += siblingIdx);
			for (auto e = parent->children.end(); b < e; b++)
				(*b)->siblingIdx--;
			onDistacted(*parent);
		}
		parent = NULL;
	}
	siblingIdx = -1;
}

void Object::addChild(Object & child)
{
	child.unparent();
	child.parent = this;
	child.siblingIdx = children.size();
	children.push_back(&child);
}

void Object::clearChild()
{
	for (auto b = children.begin(), e = children.end(); b != e; b++) {
		(*b)->parent = NULL;
		(*b)->siblingIdx = -1;
		(*b)->onDistacted(*this);
	}
	children.clear();
}

void Object::traverse(void(*preFunc)(Object &object), void(*postFunc)(Object &object))
{
	for (auto b = children.begin(), e = children.end(); b != e; b++) {
		preFunc(**b);
		(*b)->traverse(preFunc, postFunc);
		postFunc(**b);
	}
}

void Object::traverse(bool(*preFunc)(Object &object), void(*postFunc)(Object &object, bool))
{
	for (auto b = children.begin(), e = children.end(); b != e; b++) {
		bool run = preFunc(**b);
		if (run)
			(*b)->traverse(preFunc, postFunc);
		postFunc(**b, run);
	}
}

void Object::preTraverse(void(*func)(Object &object))
{
	for (auto b = children.begin(), e = children.end(); b != e; b++) {
		func(**b);
		(*b)->postTraverse(func);
	}
}

void Object::preTraverse(bool(*func)(Object &object))
{
	auto b = children.begin();
	while (b != children.end()) {
		if (func(**b)) {
			Object* root = getRoot();
			for (auto _b = (*b)->children.begin(), _e = (*b)->children.end(); _b != _e; _b++)
				(*_b)->setParent(*root);
			delete * b;
			b = children.erase(b);
		}
		else {
			(*b)->preTraverse(func);
			b++;
		}
	}
}

void Object::preTraverse(bool(*func)(Object &object, float), float arg)
{
	auto b = children.begin();
	while (b != children.end()) {
		if (func(**b, arg)) {
			Object* root = getRoot();
			for (auto _b = (*b)->children.begin(), _e = (*b)->children.end(); _b != _e; _b++)
				(*_b)->setParent(*root);
			b = children.erase(b);
			Brane::vanish(typeid(Object).hash_code(), *b);
		}
		else {
			(*b)->preTraverse(func, arg);
			b++;
		}
	}
}

void Object::postTraverse(void(*func)(Object &object))
{
	for (auto b = children.begin(), e = children.end(); b != e; b++) {
		(*b)->postTraverse(func);
		func(**b);
	}
}

void Object::destroy(bool applyToChild)
{
	tryDestroy = true;
	/*unparent();
	if (applyToChild)
		for (auto b = children.begin(), e = children.end(); b != e; b++)
			(*b)->destroy(applyToChild);
	else
		clearChild();*/
}

bool Object::isDestroy()
{
	return tryDestroy;
}

bool Object::isinitialized()
{
	return initialized;
}

Serializable * Object::instantiate(const SerializationInfo & from)
{
	Object* obj = new Object(from.name);
	ChildrenInstantiate(Object, from, obj);
	return obj;
}

bool Object::deserialize(const SerializationInfo & from)
{
	const SerializationInfo* child = from.get("children");
	if (child != NULL) {
		for (int i = 0; i < children.size(); i++) {
			const SerializationInfo* info = child->get(children[i]->name);
			if (info != NULL)
				children[i]->deserialize(*info);
		}
	}
	return true;
}

bool Object::serialize(SerializationInfo & to)
{
	to.type = "Object";
	to.name = name;
	SerializationInfo* child = to.add("children");
	if (child != NULL)
		for (int i = 0; i < children.size(); i++) {
			SerializationInfo* info = child->add(children[i]->name);
			if (info != NULL) {
				children[i]->serialize(*info);
			}
		}
	return true;
}

ObjectIterator::ObjectIterator(Object * root)
{
	this->root = root;
	curObj = root;
}

bool ObjectIterator::next()
{
	if (delay) {
		delay = false;
		return curObj != NULL;
	}
	Object* o = curObj->getChild();
	if (o == NULL) {
		if (curObj == root)
			return false;
		o = curObj->getSibling();
		if (o == NULL) {
			o = curObj->parent;
			while (o != NULL) {
				if (o == root)
					return false;
				Object* s = o->getSibling();
				if (s == NULL)
					o = o->parent;
				else {
					curObj = s;
					return true;
				}
			}
			return false;
		}
		else {
			curObj = o;
			return true;
		}
	}
	else {
		curObj = o;
		return true;
	}
}

Object & ObjectIterator::current()
{
	return *curObj;
}

Object * ObjectIterator::unparentCurrent()
{
	Object* root = curObj->getRoot();
	for (auto b = curObj->children.begin(), e = curObj->children.end(); b != e; b++)
		(*b)->unparent();
	Object* o = curObj->parent, *bak = curObj;
	while (o != NULL) {
		Object* s = o->getSibling();
		if (s == NULL)
			o = o->parent;
		else {
			o = s;
			break;
		}
	}
	curObj->unparent();
	curObj = o;
	delay = true;
	return bak;
}

void ObjectIterator::reset(Object * root)
{
	if (root == NULL)
		curObj = this->root;
	else
		curObj = this->root = root;
	delay = false;
}