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

class Engine {
public:

    void Init(const WindowInfo& info);
    void Update();

    void LateUpdate();
    const WindowInfo& GetWindow() { return _window; }
    shared_ptr<Device> GetDevice() { return _device; }
    shared_ptr<CommandQueue> GetCmdQueue() { return _cmdQueue; }
    shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
    shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
    shared_ptr<TableDescriptorHeap> GetTableDescriptorHeap() { return _tableDescHeap; }

    shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
    shared_ptr<RenderTargetGroup> GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE type) { return _renderTargetGroups[static_cast<uint8>(type)]; }

    void Render();
    void RenderBegin();
    void RenderEnd();

    void ResizeWindow(int32 width, int32 height);

private:
    void showFps();
    void CreateConstantBuffer(CBV_REGISTER reg, uint32 buffersize, uint32 count);
    void CreateRenderTargetGroups();

    // 그려질 화면 크기 관련
    WindowInfo		_window;
    D3D12_VIEWPORT	_viewport = {};
    D3D12_RECT		_scissorRect = {};

    shared_ptr<Device> _device = make_shared<Device>();
    shared_ptr<CommandQueue> _cmdQueue = make_shared<CommandQueue>();
    shared_ptr<SwapChain> _swapChain = make_shared<SwapChain>();
    shared_ptr<RootSignature> _rootSignature = make_shared<RootSignature>();
    shared_ptr<TableDescriptorHeap> _tableDescHeap = make_shared<TableDescriptorHeap>();

    vector<shared_ptr<ConstantBuffer>> _constantBuffers;
    array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _renderTargetGroups;
};
