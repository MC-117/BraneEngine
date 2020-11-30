#pragma once
#ifndef _IVENDOR_H_
#define _IVENDOR_H_

#include "ITexture.h"

struct EngineConfig
{
	string vendorName = "__Auto__";
	bool fullscreen = false;
	bool guiOnly = false;
	bool loadDefaultAsset = true;
	bool loadEngineAsset = true;
	bool loadContentAsset = true;
	bool vsnyc = false;
	unsigned int screenWidth = 960, screenHeight = 640;
};

struct WindowContext
{
	Unit2Di screenPos;
	Unit2Di screenSize;
	Unit2Di fullscreenSize;
	string srceenTile;
	HWND _hwnd;
	HINSTANCE _hinstance;
	void* window;
	bool _fullscreen;
#ifdef UNICODE
	wstring executionPath;
#else
	string executionPath;
#endif // UNICODE
};

class VendorManager;

#define REG_VENDOR_DEC(type)\
class type##Register\
{\
public:\
	type##Register();\
private:\
	static type##Register vendorRegister;\
};\

#define REG_VENDOR_IMP(type, name)\
type##Register type##Register::vendorRegister;\
type##Register::type##Register()\
{\
	VendorManager::getInstance().regist(name, []()->IVendor* { return new type(); });\
}\

class IVendor
{
public:
	IVendor();
	virtual ~IVendor();

	string getName();
	virtual bool windowSetup(EngineConfig& config, WindowContext& context);
	virtual bool setup(const EngineConfig& config, const WindowContext& context);
	virtual bool imGuiInit(const EngineConfig& config, const WindowContext& context);
	virtual bool imGuiNewFrame(const EngineConfig& config, const WindowContext& context);
	virtual bool imGuiDrawFrame(const EngineConfig& config, const WindowContext& context);
	virtual bool swap(const EngineConfig& config, const WindowContext& context);
	virtual bool imGuiShutdown(const EngineConfig& config, const WindowContext& context);
	virtual bool clean(const EngineConfig& config, const WindowContext& context);

	virtual bool guiOnlyRender(const Color& clearColor);
	virtual bool resizeWindow(const EngineConfig& config, const WindowContext& context, unsigned int width, unsigned int height);

	virtual ITexture2D* newTexture2D(Texture2DDesc& desc) const = 0;
protected:
	string name;
};

class VendorManager
{
public:
	typedef IVendor*(*VendorRegistFunc)();

	void regist(const string& name, VendorRegistFunc func);

	static VendorManager& getInstance();
	IVendor& getVendor();
	void instantiateVendor(const string& name);
	void instantiateVendor(unsigned int index);
protected:
	map<string, unsigned int> vendorNames;
	vector<VendorRegistFunc> vendorFactories;
	IVendor* activedVendor = NULL;
};

#endif // !_IVENDOR_H_
