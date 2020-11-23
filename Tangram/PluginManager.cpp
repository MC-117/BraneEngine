#define _AFXDLL
#include <afxdlgs.h>
#include "PluginManager.h"

map<string, PluginBase*> PluginManager::plugins;

void PluginManager::loadPluginFolder(const string & path)
{
	namespace FS = experimental::filesystem;
	for (auto& p : FS::directory_iterator(path)) {
		string path = p.path().generic_string();
		USES_CONVERSION;
		wchar_t* pathwc = A2W(path.c_str());
		if (PathIsDirectoryW(p.path().c_str())) {
			string name = p.path().filename().generic_string();
			FS::path dll = p.path();
			dll += ('\\' + name + ".dll");
			if (PathFileExistsW(dll.c_str())) {
				if (loadPlugin(dll.generic_string()))
					Console::log("PluginManager: load \"%s\"", name.c_str());
				else
					Console::error("PluginManager: load \"%s\" failed", dll.generic_string().c_str());
			}
			else
				Console::warn("PluginManager: missing \"%s\"", dll.generic_string().c_str());
		}
	}
}

bool PluginManager::loadPlugin(const string & path)
{
	experimental::filesystem::path p = path;
	string folder = p.parent_path().generic_string();
	char pbackup[1024];
	GetCurrentDirectoryA(1024, pbackup);
	SetCurrentDirectoryA(folder.c_str());
	HMODULE hPlugin = LoadLibraryA(path.c_str());
	if (hPlugin == NULL)
		return false;
	PluginInstanceFunc func = (PluginInstanceFunc)GetProcAddress(hPlugin, "instancePlugin");
	if (func != NULL) {
		PluginBase* plugin = func();
		if (plugin != NULL) {
			if (plugin->getEngineVersion() == ENGINE_VERSION) {
				plugin->setWorkPath(folder);
				plugin->setHModule(hPlugin);
				if (plugin->onInitialize()) {
					plugins[plugin->name] = plugin;
					SetCurrentDirectoryA(pbackup);
					return true;
				}
				else {
					FreeLibrary(hPlugin);
					SetCurrentDirectoryA(pbackup);
					return false;
				}
			}
			else {
				FreeLibrary(hPlugin);
				SetCurrentDirectoryA(pbackup);
				return false;
			}
		}
		else {
			FreeLibrary(hPlugin);
			SetCurrentDirectoryA(pbackup);
			return false;
		}
	}
	FreeLibrary(hPlugin);
	SetCurrentDirectoryA(pbackup);
	return false;
}

void PluginManager::onDestroy()
{
	for (auto b = plugins.begin(), e = plugins.end(); b != e; b++) {
		b->second->onDestroy();
	}
}

void PluginManager::beforeWorldInitialze(World& world)
{
	for (auto b = plugins.begin(), e = plugins.end(); b != e; b++) {
		b->second->beforeWorldInitialize(world);
	}
}

void PluginManager::afterWorldInitialze(World& world)
{
	for (auto b = plugins.begin(), e = plugins.end(); b != e; b++) {
		b->second->afterWorldInitialize(world);
	}
}
