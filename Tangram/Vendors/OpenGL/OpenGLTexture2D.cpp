#include "OpenGLTexture2D.h"

OpenGLTexture2D::OpenGLTexture2D(Texture2DDesc& desc) : ITexture2D(desc), info(desc.info)
{
	if (desc.bindType == 0)
		desc.bindType = GL_TEXTURE_2D;
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	if (desc.textureHandle != 0)
		glDeleteTextures(1, &desc.textureHandle);
}

bool OpenGLTexture2D::isValid() const
{
	return desc.data != NULL || desc.textureHandle != 0;
}

void OpenGLTexture2D::release()
{
	if (desc.textureHandle != 0) {
		glDeleteTextures(1, &desc.textureHandle);
		desc.textureHandle = 0;
	}
}

unsigned int OpenGLTexture2D::bind()
{
	if (desc.textureHandle != 0) {
		glBindTexture(desc.bindType, desc.textureHandle);
		return desc.textureHandle;
	}
	glGenTextures(1, &desc.textureHandle);
	if (desc.textureHandle == 0)
		return 0;
	glBindTexture(desc.bindType, desc.textureHandle);
	glTexParameteri(desc.bindType, GL_TEXTURE_WRAP_S, info.wrapSType);
	glTexParameteri(desc.bindType, GL_TEXTURE_WRAP_T, info.wrapTType);
	glTexParameteri(desc.bindType, GL_TEXTURE_MIN_FILTER, info.minFilterType);
	glTexParameteri(desc.bindType, GL_TEXTURE_MAG_FILTER, info.magFilterType);
	if (info.wrapSType == GL_CLAMP_TO_BORDER || info.wrapTType == GL_CLAMP_TO_BORDER) {
		glTexParameterfv(desc.bindType, GL_TEXTURE_BORDER_COLOR, (float*)&info.borderColor);
	}
	GLenum colorType = getColorType(desc.channel, info.internalType);
	if (desc.mipLevel > 1) {
		glTexParameteri(desc.bindType, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(desc.bindType, GL_TEXTURE_MAX_LEVEL, desc.mipLevel - 1);
	}
	GLenum dataType = colorType == GL_DEPTH_COMPONENT ? GL_FLOAT : GL_UNSIGNED_BYTE;
	if (desc.autoGenMip) {
		if (desc.width % 4 != 0 || desc.height % 4 != 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(desc.bindType, 0, info.internalType, desc.width, desc.height, 0, colorType, dataType, desc.data);
		glGenerateMipmap(desc.bindType);
		desc.mipLevel = 1 + floor(log2(max(desc.width, desc.height)));
	}
	else {
		int offset = 0;
		int _width = desc.width, _height = desc.height;
		for (int level = 0; level < desc.mipLevel; level++) {
			if (_width % 4 != 0 || _height % 4 != 0)
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			else
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTexImage2D(desc.bindType, level, info.internalType, _width, _height, 0, colorType, dataType, desc.data + offset);
			offset += desc.channel * _width * _height * sizeof(char);
			_width /= 2, _height /= 2;
		}
	}
	return desc.textureHandle;
}

unsigned int OpenGLTexture2D::resize(unsigned int width, unsigned int height)
{
	if (desc.width == width && desc.height == height && desc.textureHandle != 0)
		return desc.textureHandle;
	if (desc.data == NULL) {
		ITexture::resize(width, height);
		if (desc.textureHandle != 0) {
			glBindTexture(desc.bindType, 0);
			glDeleteTextures(1, &desc.textureHandle);
			desc.textureHandle = 0;
		}
		bind();
		glBindTexture(desc.bindType, 0);
	}
	return desc.textureHandle;
}

bool OpenGLTexture2D::assign(unsigned int width, unsigned int height, unsigned channel, const Texture2DInfo & info, unsigned int texHandle, unsigned int bindType)
{
	if (desc.data != NULL)
		return false;
	desc.width = width;
	desc.height = height;
	desc.channel = channel;
	desc.info = info;
	this->info = info;
	desc.bindType = bindType;
	desc.textureHandle = texHandle;
	return true;
}

GLenum OpenGLTexture2D::getColorType(unsigned int channel, GLenum& internalType)
{
	if (internalType == GL_NONE) {
		switch (channel)
		{
		case 1:
			internalType = GL_RED;
			break;
		case 2:
			internalType = GL_RG;
			break;
		case 3:
			internalType = GL_RGB;
			break;
		case 4:
			internalType = GL_RGBA;
			break;
		}
		return internalType;
	}
	else if (internalType == GL_DEPTH_COMPONENT24)
		return GL_DEPTH_COMPONENT;
	else if (internalType == GL_SRGB)
		return GL_RGB;
	return internalType;
}

GLenum OpenGLTexture2DInfo::toGLWrapType(const TexWrapType & type)
{
	switch (type)
	{
	case TW_Repeat:
		return GL_REPEAT;
	case TW_Mirror:
		return GL_MIRRORED_REPEAT;
	case TW_Clamp:
		return GL_CLAMP;
	case TW_Clamp_Edge:
		return GL_CLAMP_TO_EDGE;
	case TW_Border:
		return GL_CLAMP_TO_BORDER;
	case TW_Mirror_Once:
		return GL_MIRROR_CLAMP_TO_EDGE;
	}
	return GL_NONE;
}

GLenum OpenGLTexture2DInfo::toGLFilterType(const TexFilter & type)
{
	switch (type)
	{
	case TF_Point:
		return GL_NEAREST;
	case TF_Linear:
		return GL_LINEAR;
	case TF_Point_Mip_Point:
		return GL_NEAREST_MIPMAP_NEAREST;
	case TF_Linear_Mip_Point:
		return GL_LINEAR_MIPMAP_NEAREST;
	case TF_Point_Mip_Linear:
		return GL_NEAREST_MIPMAP_LINEAR;
	case TF_Linear_Mip_Linear:
		return GL_LINEAR_MIPMAP_LINEAR;
	}
	return GL_NONE;
}

GLenum OpenGLTexture2DInfo::toGLInternalType(const TexInternalType & type)
{
	switch (type)
	{
	case TIT_Default:
		return GL_NONE;
	case TIT_R:
		return GL_RED;
	case TIT_RG:
		return GL_RG;
	case TIT_RGB:
		return GL_RGB;
	case TIT_RGBA:
		return GL_RGBA;
	case TIT_SRGB:
		return GL_SRGB;
	case TIT_Depth:
		return GL_DEPTH_COMPONENT24;
	}
	return GL_NONE;
}

OpenGLTexture2DInfo::OpenGLTexture2DInfo(const Texture2DInfo & info)
{
	wrapSType = toGLWrapType(info.wrapSType);
	wrapTType = toGLWrapType(info.wrapTType);
	minFilterType = toGLFilterType(info.minFilterType);
	magFilterType = toGLFilterType(info.magFilterType);
	internalType = toGLInternalType(info.internalType);
	borderColor = info.borderColor;
}
