#pragma once

#include "../../ITexture.h"

#ifdef VENDOR_USE_OPENGL

#ifndef _OPENGLTEXTURE2D_H_
#define _OPENGLTEXTURE2D_H_

struct OpenGLTexture2DInfo {
	GLenum wrapSType;
	GLenum wrapTType;
	GLenum minFilterType;
	GLenum magFilterType;
	GLenum internalType = NULL;
	Color borderColor = { 0, 0, 0, 0 };

	static GLenum toGLWrapType(const TexWrapType& type);
	static GLenum toGLFilterType(const TexFilter& type);
	static GLenum toGLInternalType(const TexInternalType& type);

	OpenGLTexture2DInfo(const Texture2DInfo& info);
};

class OpenGLTexture2D : public ITexture2D
{
public:
	OpenGLTexture2DInfo info;

	OpenGLTexture2D(Texture2DDesc& desc);
	~OpenGLTexture2D();

	virtual bool isValid() const;

	virtual void release();

	virtual unsigned int bind();
	virtual unsigned int resize(unsigned int width, unsigned int height);

	virtual bool assign(unsigned int width, unsigned int height, unsigned channel, const Texture2DInfo& info, unsigned int texHandle, unsigned int bindType);

	static GLenum getColorType(unsigned int channel, GLenum& internalType);
};

#endif // !_OPENGLTEXTURE2D_H_

#endif // VENDOR_USE_OPENGL