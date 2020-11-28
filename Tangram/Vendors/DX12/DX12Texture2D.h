#pragma once
#include "../../ITexture.h"

#ifdef VENDOR_USE_DX12

#ifndef _DX12TEXTURE2D_H_
#define _DX12TEXTURE2D_H_

#include "DX12.h"

struct DX12Texture2DInfo
{
	D3D12_RESOURCE_DESC texture2DDesc;
	D3D12_SAMPLER_DESC samplerDesc;
	DX12Texture2DInfo(const Texture2DInfo& info);

	static D3D12_TEXTURE_ADDRESS_MODE toDX12WrapType(const TexWrapType& type);
	static D3D12_FILTER toDX12FilterType(const TexFilter& minType, const TexFilter& magType);
	static DXGI_FORMAT toGLInternalType(const TexInternalType& type);
};

class DX12Texture2D : public ITexture2D
{
public:
	DX12Texture2DInfo info;

	DX12Texture2D(Texture2DDesc& desc);
	~DX12Texture2D();

	virtual bool isValid() const;

	virtual void release();

	virtual unsigned int bind();
	virtual unsigned int resize(unsigned int width, unsigned int height);

	virtual bool assign(unsigned int width, unsigned int height, unsigned channel, const Texture2DInfo& info, unsigned int texHandle, unsigned int bindType);
};

#endif // !_DX12TEXTURE2D_H_

#endif // VENDOR_USE_DX12