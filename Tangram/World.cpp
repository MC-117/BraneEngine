#include "World.h"
//#include "MaterialLoader.h"
#include <fstream>
#include "Utility.h"

SerializeInstance(World);

Timer timer;

#ifdef PHYSICS_USE_BULLET
void physicalTick(World* w)
{
	float dtime = 1 / 60.f;
	while (!w->willQuit()) {
		if (w != NULL) {
			Time s = Time::now();
			w->physicalWorld.updatePhysicalWorld(dtime);
			w->physicalWorld.collisionDispatch();
			Time e = Time::now();
			float lt = dtime * 1000 - (e - s);
			if (lt > 0)
				Sleep(lt * 0.375);
		}
		else
			break;
	}
}
#endif

void renderTick(World* w)
{
	while (!w->willQuit())
		if (w != NULL)
			w->renderPool.render();
}

World::World() : ::Transform::Transform("RootWorld")
{
}

World::~World()
{
}

void World::begin()
{
	lastTime = currentTime = startTime = getCurrentTime();
	if (defaultCamera.parent == NULL)
		*this += defaultCamera;
	*this += input;
	physicalWorld.setGravity({ 0.f, 0.f, -10.f });
	::Transform::begin();
	iter.reset();
	while (iter.next())
		iter.current().begin();
}

void World::tick(float deltaTime)
{
	timer.reset();
	currentTime = getCurrentTime();
	float dt = (currentTime - lastTime) / 1000.0f;
	lastTime = currentTime;
	//if (!destroyList.empty()) {
	//	for (auto b = destroyList.begin(), e = destroyList.end(); b != e; b++) {
	//		Render* rd = (*b)->getRender();
	//		PhysicalBody* p = (PhysicalBody*)(*b)->getPhysicalBody();
	//		if (rd != NULL)
	//			renderPool.remove(*rd);
	//		if (p != NULL)
	//			physicalWorld.removePhysicalBody(*p);
	//		//Brane::vanish(typeid(Object).hash_code(), &obj);
	//		delete *b;
	//	}
	//	destroyList.clear();
	//}
	::Transform::tick(dt);
	iter.reset();
	while (iter.next()) {
		Object& obj = iter.current();
		vector<Render*> rds;
		obj.getRenders(rds);
		if (obj.isDestroy()) {
			//destroyList.push_back(&obj);
			iter.unparentCurrent();
			for (int i = 0; i < rds.size(); i++)
				renderPool.remove(*rds[i]);
			obj.releasePhysics(physicalWorld);
			//Brane::vanish(typeid(Object).hash_code(), &obj);
			delete &obj;
		}
		else {
			for (int i = 0; i < rds.size(); i++)
				if (rds[i]->renderPool == NULL)
					renderPool.add(*rds[i]);
			obj.setupPhysics(physicalWorld);
			if (!obj.isinitialized())
				obj.begin();
			obj.tick(dt);
		}
	}
#ifdef PHYSICS_USE_BULLET
	if (physicalWorldThread == NULL) {
		physicalWorldThread = new thread(physicalTick, this);
		physicalWorldThread->detach();
	}
#endif
#ifdef PHYSICS_USE_PHYSX
	physicalWorld.updatePhysicalWorld(dt);
#endif // !PHYSICS_USE_PHYSX
	/*physicalWorld.updatePhysicalWorld(deltaTime);
	physicalWorld.collisionDispatch();*/
	timer.record("Tick");
}

void World::afterTick()
{
	::Transform::afterTick();
	iter.reset();
	while (iter.next())
		iter.current().afterTick();
	timer.record("Aftertick");
	/*if (renderThread == NULL) {
		renderThread = new thread(renderTick, this);
		renderThread->detach();
	}*/
	updateListener();
	iter.reset();
	while (iter.next())
		iter.current().prerender();
	timer.record("PreRender");

	renderPool.render(guiOnly);
	timer.record("Render");
	Console::getTimer("Game") = timer;
	/*physicalWorld.db.SetMatrices(getCurrentCamera().getViewMatrix(), getCurrentCamera().getProjectionMatrix());
	physicalWorld.physicsScene->debugDrawWorld();*/
	//renderPool.gui.render();
	/*glutSwapBuffers();*/
}

void World::end()
{
	iter.reset();
	while (iter.next()) {
		Object* obj = &iter.current();
		//Brane::vanish(typeid(Object).hash_code(), obj);
		delete obj;
	}
#ifdef PHYSICS_USE_BULLET
	if (physicalWorldThread != NULL)
		delete[] physicalWorldThread;
#endif
	if (renderThread != NULL)
		delete renderThread;
	::Transform::end();
}

void World::quit()
{
	isQuit = true;
}

bool World::willQuit()
{
	return isQuit;
}

string World::addObject(Object & object)
{
	//Brane::born(typeid(Object).hash_code(), &object, object.path);
	addChild(object);
	/*Render* r = object.getRender();
	if (r != NULL)
		renderPool += *r;*/
	vector<Render*> rds;
	object.getRenders(rds);
	for (int i = 0; i < rds.size(); i++)
		if (rds[i]->renderPool == NULL)
			renderPool.add(*rds[i]);
	return object.name;
}

string World::addObject(Object * object)
{
	//Brane::born(typeid(Object).hash_code(), object, object->path);
	addChild(*object);
	/*Render* r = object->getRender();
	if (r != NULL)
		renderPool += *r;*/
	vector<Render*> rds;
	object->getRenders(rds);
	for (int i = 0; i < rds.size(); i++)
		if (rds[i]->renderPool == NULL)
			renderPool.add(*rds[i]);
	return object->name;
}

void World::destroyObject(string name)
{
	Object* obj = ((Object*)Brane::find(typeid(Object).hash_code(), name));
	if (obj != NULL)
		obj->destroy();
}

void World::setGUIOnly(bool value)
{
	guiOnly = value;
}

bool World::getGUIOnly()
{
	return guiOnly;
}

void World::addUIControl(UIControl & uc)
{
	renderPool.gui += uc;
}

void World::addUIControl(UIControl * uc)
{
	renderPool.gui += uc;
}

void World::bindUI(void(*uiDraw)(GUI&))
{
	renderPool.gui.uiHandler += uiDraw;
}

Camera & World::getCurrentCamera()
{
	if (camera == NULL)
		switchToDefaultCamera();
	return *camera;
}

Camera & World::getDefaultCamera()
{
	return defaultCamera;
}

void World::switchCamera(Camera & camera)
{
	this->camera = &camera;
	camera.setSize(screenSize);
	renderPool.switchCamera(camera);
}

void World::switchToDefaultCamera()
{
	camera = &defaultCamera;
	defaultCamera.setSize(screenSize);
	renderPool.switchToDefaultCamera();
}

void World::updateListener()
{
	if (camera != NULL) {
		Vector3f pos = camera->getPosition(WORLD);
		alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
		Vector3f f = camera->getForward(WORLD), u = camera->getUpward(WORLD);
		float v[6] = { f[0], f[1], f[2], u[0], u[1], u[2] };
		alListenerfv(AL_ORIENTATION, v);
	}
}

void World::setMainVolume(float v)
{
	alListenerf(AL_GAIN, v);
}

void World::setViewportSize(int width, int height)
{
	screenSize = { width, height };
	renderPool.setViewportSize(screenSize);
}

int64_t World::getEngineTime()
{
	return lastTime - startTime;
}

int64_t World::getSystemTime()
{
	return currentTime;
}

bool World::loadTransform(const string & path)
{
	ifstream file(path);
	if (file.fail()) {
		cout << "Load world transform failed\n";
		return false;
	}
	string line;
	while (getline(file, line)) {
		vector<string> strs = split(line, ':');
		if (strs.size() != 2)
			continue;
		Transform *t = dynamic_cast<Transform*>((Object*)Brane::find(typeid(Object).hash_code(), trim(strs[0], ' ')));
		if (t == NULL) {
			cout << "Cannot find (Object) " + strs[0] << endl;
			continue;
		}
		if (t == &getCurrentCamera()) {
			continue;
		}
		strs = split(strs[1], ',');
		if (strs.size() != 9)
			continue;
		float data[9];
		for (int i = 0; i < 9; i++) {
			data[i] = stof(strs[i]);
		}
		t->setPosition(data[0], data[1], data[2]);
		t->setRotation(data[3], data[4], data[5]);
		t->setScale(data[6], data[7], data[8]);
	}
	file.close();
	return true;
}

bool World::saveTransform(const string & path)
{
	string data;
	ObjectIterator iter = ObjectIterator(this);
	iter.reset();
	while (iter.next()) {
		Transform* t = dynamic_cast<Transform*>(&iter.current());
		if (t != NULL) {
			char str[1000];
			Vector3f rot = t->getEulerAngle();
			sprintf(str, ": %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t->position.x(), t->position.y(), t->position.z(),
				rot.x(), rot.y(), rot.z(), t->scale.x(), t->scale.y(), t->scale.z());
			data += t->name + str;
		}
	}
	std::ofstream file(path);
	if (file.fail()) {
		cout << "Save world transform failed\n";
		return false;
	}
	file << data;
	file.close();
	return true;
}

World & World::operator+=(Object & object)
{
	addObject(object);
	return *this;
}

World & World::operator+=(Object * object)
{
	addObject(object);
	return *this;
}

World & World::operator+=(UIControl & uc)
{
	addUIControl(uc);
	return *this;
}

World & World::operator+=(UIControl * uc)
{
	addUIControl(uc);
	return *this;
}

void World::loadWorld(const SerializationInfo & from)
{
	ChildrenInstantiate(Object, from, this);
}

Serializable * World::instantiate(const SerializationInfo & from)
{
	World* w = new World();
	ChildrenInstantiate(Object, from, w);
	return w;
}

bool World::deserialize(const SerializationInfo & from)
{
	if (!::Transform::deserialize(from))
		return false;
	return true;
}

bool World::serialize(SerializationInfo & to)
{
	if (!::Transform::serialize(to))
		return false;
	to.type = "World";
	return true;
}

int64_t World::getCurrentTime()
{
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}
