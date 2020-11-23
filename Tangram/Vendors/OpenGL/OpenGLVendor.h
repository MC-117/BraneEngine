#pragma once
#include "../../IVendor.h"

#ifdef VENDOR_USE_OPENGL

#ifndef _OPENGLVENDOR_H_
#define _OPENGLVENDOR_H_

class OpenGLVendor : public IVendor
{
public:
	OpenGLVendor();
	virtual ~OpenGLVendor();

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

	virtual ITexture2D* newTexture2D(Texture2DDesc& desc) const;
};

#endif // !_OPENGLVENDOR_H_

#endif // VENDOR_USE_OPENGL