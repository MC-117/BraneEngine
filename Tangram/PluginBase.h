#pragma once
#ifndef _PLUGINBASE_H_
#define _PLUGINBASE_H_

#define PLUGIN_VERSION "0.1.0"
#define PLUGIN_NAME "Base"

#include "World.h"

typedef void(*PluginVoidFunc)();
typedef void(*PluginPointerFunc)(void*);
typedef void(*PluginObjectFunc)(Object*);
typedef void(*PluginFloatFunc)(float);
typedef void(*PluginIntFunc)(int);
typedef void(*PluginStringFunc)(string);

class PluginBase
{
public:
	string version = PLUGIN_VERSION;
	string name = PLUGIN_NAME;
	virtual bool onInitialize();
	virtual void onDestroy();
	virtual bool beforeWorldInitialize(World& world);
	virtual bool afterWorldInitialize(World& world);
	virtual void onWorldUnload(World& world);

	virtual const string& getEngineVersion() final;
	virtual void setWorkPath(const string& path) final;
	virtual const string& getWorkPath() final;
	virtual void setHModule(HMODULE hmodule) final;
	virtual bool call(const string& name) final;
	virtual bool call(const string& name, void* ptr) final;
	virtual bool call(const string& name, Object* obj) final;
	virtual bool call(const string& name, float v) final;
	virtual bool call(const string& name, int v) final;
	virtual bool call(const string& name, string str) final;
private:
	const string _engineVersion = ENGINE_VERSION;
	string workPath;
	HMODULE hmodule = NULL;
protected:
	Events events = Events(this);
};

#endif // !_PLUGINBASE_H_
