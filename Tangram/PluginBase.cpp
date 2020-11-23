#include "PluginBase.h"

bool PluginBase::onInitialize()
{
	return true;
}

void PluginBase::onDestroy()
{
}

bool PluginBase::beforeWorldInitialize(World & world)
{
	return true;
}

bool PluginBase::afterWorldInitialize(World & world)
{
	return true;
}

void PluginBase::onWorldUnload(World & world)
{
}

const string & PluginBase::getEngineVersion()
{
	return _engineVersion;
}

void PluginBase::setWorkPath(const string & path)
{
	workPath = path;
}

const string & PluginBase::getWorkPath()
{
	return workPath;
}

void PluginBase::setHModule(HMODULE hmodule)
{
	this->hmodule = hmodule;
}

bool PluginBase::call(const string & name)
{
	return events.call(name);
}

bool PluginBase::call(const string & name, void * ptr)
{
	return events.call(name, ptr);
}

bool PluginBase::call(const string & name, Object * obj)
{
	return events.call(name, obj);
}

bool PluginBase::call(const string & name, float v)
{
	return events.call(name, v);
}

bool PluginBase::call(const string & name, int v)
{
	return events.call(name, v);
}

bool PluginBase::call(const string & name, string str)
{
	return events.call(name, str);
}
