#include "DX12Vendor.h"
#include "../../imgui_impl_glfw.h"
#include "../../imgui_impl_dx12.h"

#ifdef VENDOR_USE_DX12

REG_VENDOR_DEC(DX12Vendor);
REG_VENDOR_IMP(DX12Vendor, "DX12");

DX12Vendor::DX12Vendor()
{
	name = "DX12";
}

DX12Vendor::~DX12Vendor()
{
}

bool DX12Vendor::windowSetup(EngineConfig & config, WindowContext & context)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return true;
}

bool DX12Vendor::setup(const EngineConfig & config, const WindowContext & context)
{
	if (!createDevice(context._hwnd))
		return false;
	return true;
}

bool DX12Vendor::imGuiInit(const EngineConfig & config, const WindowContext & context)
{
	if (!ImGui_ImplGlfw_InitForDX12((GLFWwindow*)context.window, true))
		return false;
	if (!ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart()))
		return false;
}

bool DX12Vendor::imGuiNewFrame(const EngineConfig & config, const WindowContext & context)
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	return true;
}

bool DX12Vendor::imGuiDrawFrame(const EngineConfig & config, const WindowContext & context)
{

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
	renderContext.barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	renderContext.barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	g_pd3dCommandList->ResourceBarrier(1, &renderContext.barrier);
	g_pd3dCommandList->Close();

	g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);
	return true;
}

bool DX12Vendor::swap(const EngineConfig & config, const WindowContext & context)
{
	g_pSwapChain->Present(config.vsnyc ? 0 : 1, 0);

	UINT64 fenceValue = g_fenceLastSignaledValue + 1;
	g_pd3dCommandQueue->Signal(g_fence, fenceValue);
	g_fenceLastSignaledValue = fenceValue;
	renderContext.frameCtxt->FenceValue = fenceValue;
	return true;
}

bool DX12Vendor::imGuiShutdown(const EngineConfig & config, const WindowContext & context)
{
	WaitForLastSubmittedFrame();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	return true;
}

bool DX12Vendor::clean(const EngineConfig & config, const WindowContext & context)
{
	CleanupDevice();
	return true;
}

bool DX12Vendor::guiOnlyRender(const Color & clearColor)
{
	renderContext.frameCtxt = WaitForNextFrameResources();
	renderContext.backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
	renderContext.frameCtxt->CommandAllocator->Reset();

	renderContext.barrier = {};
	renderContext.barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	renderContext.barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	renderContext.barrier.Transition.pResource = g_mainRenderTargetResource[renderContext.backBufferIdx];
	renderContext.barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	renderContext.barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	renderContext.barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	g_pd3dCommandList->Reset(renderContext.frameCtxt->CommandAllocator, NULL);
	g_pd3dCommandList->ResourceBarrier(1, &renderContext.barrier);
	g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[renderContext.backBufferIdx], (float*)&clearColor, 0, NULL);
	g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[renderContext.backBufferIdx], FALSE, NULL);
	g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
	return true;
}

bool DX12Vendor::resizeWindow(const EngineConfig & config, const WindowContext & context, unsigned int width, unsigned int height)
{
	ImGui_ImplDX12_InvalidateDeviceObjects();
	CleanupRenderTarget();
	ResizeSwapChain(context._hwnd, width, height);
	CreateRenderTarget();
	return ImGui_ImplDX12_CreateDeviceObjects();
}

ITexture2D * DX12Vendor::newTexture2D(Texture2DDesc& desc) const
{
	return nullptr;
}

bool DX12Vendor::createDevice(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	if (enableDebugLayer)
	{
		ID3D12Debug* dx12Debug = NULL;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dx12Debug))))
		{
			dx12Debug->EnableDebugLayer();
			dx12Debug->Release();
		}
	}

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
		return false;

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
			return false;

	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
		g_pd3dCommandList->Close() != S_OK)
		return false;

	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
		return false;

	g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_fenceEvent == NULL)
		return false;

	{
		IDXGIFactory4* dxgiFactory = NULL;
		IDXGISwapChain1* swapChain1 = NULL;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
			dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK ||
			swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
			return false;
		swapChain1->Release();
		dxgiFactory->Release();
		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void DX12Vendor::CleanupDevice()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
	if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
	if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
	if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
	if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
	if (g_fence) { g_fence->Release(); g_fence = NULL; }
	if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void DX12Vendor::CreateRenderTarget()
{
	ID3D12Resource* pBackBuffer;
	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
	{
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
		g_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void DX12Vendor::CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

void DX12Vendor::WaitForLastSubmittedFrame()
{
	DX12FrameContext* frameCtxt = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64 fenceValue = frameCtxt->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtxt->FenceValue = 0;
	if (g_fence->GetCompletedValue() >= fenceValue)
		return;

	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
	WaitForSingleObject(g_fenceEvent, INFINITE);
}

DX12FrameContext * DX12Vendor::WaitForNextFrameResources()
{
	UINT nextFrameIndex = g_frameIndex + 1;
	g_frameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
	DWORD numWaitableObjects = 1;

	DX12FrameContext* frameCtxt = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64 fenceValue = frameCtxt->FenceValue;
	if (fenceValue != 0) // means no fence was signaled
	{
		frameCtxt->FenceValue = 0;
		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
		waitableObjects[1] = g_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtxt;
}

void DX12Vendor::ResizeSwapChain(HWND hWnd, int width, int height)
{
	DXGI_SWAP_CHAIN_DESC1 sd;
	g_pSwapChain->GetDesc1(&sd);
	sd.Width = width;
	sd.Height = height;

	IDXGIFactory4* dxgiFactory = NULL;
	g_pSwapChain->GetParent(IID_PPV_ARGS(&dxgiFactory));

	g_pSwapChain->Release();
	CloseHandle(g_hSwapChainWaitableObject);

	IDXGISwapChain1* swapChain1 = NULL;
	dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1);
	swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain));
	swapChain1->Release();
	dxgiFactory->Release();

	g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);

	g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	assert(g_hSwapChainWaitableObject != NULL);
}

#endif // VENDOR_USE_DX12