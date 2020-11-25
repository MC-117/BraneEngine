#include "DX12Texture2D.h"

DX12Texture2DInfo::DX12Texture2DInfo(const Texture2DInfo & info)
{
	texture2DDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texture2DDesc.Format = toGLInternalType(info.internalType);
	texture2DDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	texture2DDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.SampleDesc.Quality = 0;
	samplerDesc.Filter = toDX12FilterType(info.minFilterType, info.magFilterType);
	samplerDesc.AddressU = toDX12WrapType(info.wrapSType);
	samplerDesc.AddressV = toDX12WrapType(info.wrapTType);
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	memcpy(samplerDesc.BorderColor, &info.borderColor, sizeof(float) * 4);
}

D3D12_TEXTURE_ADDRESS_MODE DX12Texture2DInfo::toDX12WrapType(const TexWrapType & type)
{
	switch (type)
	{
	case TW_Repeat:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case TW_Mirror:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case TW_Clamp:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case TW_Clamp_Edge:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case TW_Border:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	case TW_Mirror_Once:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	}
	return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}

D3D12_FILTER DX12Texture2DInfo::toDX12FilterType(const TexFilter& minType, const TexFilter& magType)
{
	D3D12_FILTER_TYPE minFT, magFT, mipFT;
	switch (minType)
	{
	case TF_Point:
		break;
	case TF_Linear:
		minFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	case TF_Point_Mip_Point:
		break;
	case TF_Linear_Mip_Point:
		minFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	case TF_Point_Mip_Linear:
		mipFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	case TF_Linear_Mip_Linear:
		minFT = D3D12_FILTER_TYPE_LINEAR;
		mipFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	}
	switch (magType)
	{
	case TF_Point:
		break;
	case TF_Linear:
		magFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	case TF_Point_Mip_Point:
		break;
	case TF_Linear_Mip_Point:
		magFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	case TF_Point_Mip_Linear:
		mipFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	case TF_Linear_Mip_Linear:
		magFT = D3D12_FILTER_TYPE_LINEAR;
		mipFT = D3D12_FILTER_TYPE_LINEAR;
		break;
	}
	return D3D12_ENCODE_BASIC_FILTER(minFT, magFT, mipFT, 0);
}

DXGI_FORMAT DX12Texture2DInfo::toGLInternalType(const TexInternalType & type)
{
	switch (type)
	{
	case TIT_Default:
		return DXGI_FORMAT_UNKNOWN;
	case TIT_R:
		return DXGI_FORMAT_R8_UNORM;
	case TIT_RG:
		return DXGI_FORMAT_R8G8_UNORM;
	case TIT_RGBA:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case TIT_SRGBA:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case TIT_Depth:
		return DXGI_FORMAT_D32_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

DX12Texture2D::DX12Texture2D(Texture2DDesc & desc) : ITexture2D(desc), info(desc.info)
{
}

DX12Texture2D::~DX12Texture2D()
{
}

bool DX12Texture2D::isValid() const
{
	return false;
}

void DX12Texture2D::release()
{
}

unsigned int DX12Texture2D::bind()
{
	return 0;
}

unsigned int DX12Texture2D::resize(unsigned int width, unsigned int height)
{
	return 0;
}

bool DX12Texture2D::assign(unsigned int width, unsigned int height, unsigned channel, const Texture2DInfo & info, unsigned int texHandle, unsigned int bindType)
{
	return false;
}
