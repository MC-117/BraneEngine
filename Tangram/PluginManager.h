#pragma once
#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include "PluginBase.h"

typedef PluginBase*(*PluginInstanceFunc)();

class PluginManager
{
public:
	static map<string, PluginBase*> plugins;

	static void loadPluginFolder(const string& path);
	static bool loadPlugin(const string& path);
	static void onInitialize();
	static void onDestroy();
	static void beforeWorldInitialze(World& world);
	static void afterWorldInitialze(World& world);
};

#endif // !_PLUGINMANAGER_H_
