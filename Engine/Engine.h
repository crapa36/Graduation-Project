#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "Texture.h"
#include "RenderTargetGroup.h"
#include "ImguiManager.h"

class Engine {
public:

    void Init(const WindowInfo& info);
    void Update();

    void LateUpdate();
    const WindowInfo& GetWindow() { return _window; }
    shared_ptr<Device> GetDevice() { return _device; }
    shared_ptr<GraphicsCommandQueue> GetGraphicsCmdQueue() { return _graphicsCmdQueue; }
    shared_ptr<ComputeCommandQueue> GetComputeCmdQueue() { return _computeCmdQueue; }
    shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
    shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
    shared_ptr<GraphicsDescriptorHeap> GetGraphicsDescriptorHeap() { return _graphicsDescriptorHeap; }
    shared_ptr<ComputeDescriptorHeap> GetComputeDescriptorHeap() { return _computeDescriptorHeap; }

    shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
    shared_ptr<RenderTargetGroup> GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE type) { return _renderTargetGroups[static_cast<uint8>(type)]; }
    shared_ptr<ImguiDescriptorHeap> GetImguiDescriptorHeap() { return _imguiDescriptorHeap; }

    void Render();
    void RenderBegin();
    void RenderEnd();

    void ResizeWindow(int32 width, int32 height);

    void SetDebugMode(bool debug) { _debugMode = debug; }
    void SetImguiMode(bool imguiMode) { _imguiMode = imguiMode; }

    bool GetDebugMode() { return _debugMode; }
    bool GetImguiMode() { return _imguiMode; }

private:
    void ShowFps();
    void CreateConstantBuffer(CBV_REGISTER reg, uint32 buffersize, uint32 count);
    void CreateRenderTargetGroups();

    // ±×·ÁÁú È­¸é Å©±â °ü·Ã
    WindowInfo		_window;
    D3D12_VIEWPORT	_viewport = {};
    D3D12_RECT		_scissorRect = {};

    bool _debugMode = false;
    bool _imguiMode = false;

    shared_ptr<Device> _device = make_shared<Device>();
    shared_ptr<GraphicsCommandQueue> _graphicsCmdQueue = make_shared<GraphicsCommandQueue>();
    shared_ptr<ComputeCommandQueue> _computeCmdQueue = make_shared<ComputeCommandQueue>();
    shared_ptr<SwapChain> _swapChain = make_shared<SwapChain>();
    shared_ptr<RootSignature> _rootSignature = make_shared<RootSignature>();
    shared_ptr<GraphicsDescriptorHeap> _graphicsDescriptorHeap = make_shared<GraphicsDescriptorHeap>();
    shared_ptr<ComputeDescriptorHeap> _computeDescriptorHeap = make_shared<ComputeDescriptorHeap>();

    shared_ptr<ImguiDescriptorHeap> _imguiDescriptorHeap = make_shared<ImguiDescriptorHeap>();

    vector<shared_ptr<ConstantBuffer>> _constantBuffers;
    array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _renderTargetGroups;
};
