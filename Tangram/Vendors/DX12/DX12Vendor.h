#pragma once
#include "../../IVendor.h"

//#undef VENDOR_USE_DX12

#ifdef VENDOR_USE_DX12

#ifndef _DX12VENDOR_H_
#define _DX12VENDOR_H_

#include "DX12.h"

struct DX12FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

const int                   NUM_FRAMES_IN_FLIGHT = 3;
const int                   NUM_BACK_BUFFERS = 3;

class DX12Vendor : public IVendor
{
public:
	DX12Vendor();
	virtual ~DX12Vendor();

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
protected:

	struct RenderContext
	{
		DX12FrameContext* frameCtxt;
		UINT backBufferIdx;
		D3D12_RESOURCE_BARRIER barrier;
	} renderContext;

	bool                         enableDebugLayer = true;
	DX12FrameContext             g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
	UINT                         g_frameIndex = 0;

	ID3D12Device*                g_pd3dDevice = NULL;
	ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
	ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
	ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
	ID3D12Fence*                 g_fence = NULL;
	HANDLE                       g_fenceEvent = NULL;
	UINT64                       g_fenceLastSignaledValue = 0;
	IDXGISwapChain3*             g_pSwapChain = NULL;
	HANDLE                       g_hSwapChainWaitableObject = NULL;
	ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

	bool createDevice(HWND hWnd);
	void CleanupDevice();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void WaitForLastSubmittedFrame();
	DX12FrameContext* WaitForNextFrameResources();
	void ResizeSwapChain(HWND hWnd, int width, int height);
};

#endif // !_DX12VENDOR_H_

#endif // VENDOR_USE_DX12