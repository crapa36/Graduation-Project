#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Input.h"
#include "Transform.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"
#include "InstancingManager.h"

void Engine::Init(const WindowInfo& info) {
    _window = info;

    // 그려질 화면 크기를 설정
    _viewport = { 0, 0, static_cast<FLOAT>(info.clientWidth), static_cast<FLOAT>(info.clientHeight), 0.0f, 1.0f };
    _scissorRect = CD3DX12_RECT(0, 0, info.clientWidth, info.clientHeight);

    _device->Init();
    _graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
    _computeCmdQueue->Init(_device->GetDevice());
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetCmdQueue());
    _rootSignature->Init();
    _graphicsDescriptorHeap->Init(256);
    _computeDescriptorHeap->Init();

    _imguiDescriptorHeap->Init();

    CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
    CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256);
    CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);

    CreateRenderTargetGroups();

    //ResizeWindow(info.width, info.height);

    GET_SINGLETON(Input)->Init(info);
    GET_SINGLETON(Timer)->Init();
    GET_SINGLETON(Resources)->Init();
    GET_SINGLETON(SceneManager)->Init(info);
    GET_SINGLETON(ImguiManager)->Init(info.hwnd, _device->GetDevice(), *_imguiDescriptorHeap.get());
}

void Engine::Update() {
    GET_SINGLETON(Input)->Update();
    GET_SINGLETON(Timer)->Update();
    GET_SINGLETON(SceneManager)->Update();
    GET_SINGLETON(InstancingManager)->ClearBuffer();
    if (_imguiMode)
        GET_SINGLETON(ImguiManager)->Update();

    Render();

    ShowFps();
}

void Engine::Render() {
    RenderBegin();

    GET_SINGLETON(SceneManager)->Render();
    if (_imguiMode)
        GET_SINGLETON(ImguiManager)->Render();
    RenderEnd();
}

void Engine::RenderBegin() {
    _graphicsCmdQueue->RenderBegin();
}

void Engine::RenderEnd() {
    _graphicsCmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height) {
    _window.width = width;
    _window.height = height;

    RECT rect = { 0, 0, width, height };
    ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    ::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0);
    RECT clientRect;
    ::GetClientRect(_window.hwnd, &clientRect);

    _window.clientWidth = clientRect.right - clientRect.left;
    _window.clientHeight = clientRect.bottom - clientRect.top;
}

void Engine::ShowFps() {
    uint32 fps = GET_SINGLETON(Timer)->GetFPS();

    WCHAR text[100] = L"";
    ::wsprintf(text, L"FPS : %d", fps);

    ::SetWindowText(_window.hwnd, text);
}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count) {
    uint8 typeInt = static_cast<uint8>(reg);
    assert(_constantBuffers.size() == typeInt);

    shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
    buffer->Init(reg, bufferSize, count);
    _constantBuffers.push_back(buffer);
}

void Engine::CreateRenderTargetGroups() {

    // DepthStencil
    shared_ptr<Texture> dsTexture = GET_SINGLETON(Resources)->CreateTexture(L"DepthStencil",
                                                                            DXGI_FORMAT_D32_FLOAT, _window.clientWidth, _window.clientHeight,
                                                                            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    // SwapChain Group
    {
        vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

        for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
            wstring name = L"SwapChainTarget_" + std::to_wstring(i);

            ComPtr<ID3D12Resource> resource;
            _swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
            rtVec[i].target = GET_SINGLETON(Resources)->CreateTextureFromResource(name, resource);
        }

        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
    }

    // Shadow Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

        rtVec[0].target = GET_SINGLETON(Resources)->CreateTexture(L"ShadowTarget",
                                                                  DXGI_FORMAT_R32_FLOAT, 4096, 4096,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        shared_ptr<Texture> shadowDepthTexture = GET_SINGLETON(Resources)->CreateTexture(L"ShadowDepthStencil",
                                                                                         DXGI_FORMAT_D32_FLOAT, 4096, 4096,
                                                                                         CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                                         D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
    }

    // Deferred Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

        rtVec[0].target = GET_SINGLETON(Resources)->CreateTexture(L"PositionTarget",
                                                                  DXGI_FORMAT_R32G32B32A32_FLOAT, _window.clientWidth, _window.clientHeight,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[1].target = GET_SINGLETON(Resources)->CreateTexture(L"NormalTarget",
                                                                  DXGI_FORMAT_R32G32B32A32_FLOAT, _window.clientWidth, _window.clientHeight,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[2].target = GET_SINGLETON(Resources)->CreateTexture(L"DiffuseTarget",
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM, _window.clientWidth, _window.clientHeight,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
    }

    // Lighting Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

        rtVec[0].target = GET_SINGLETON(Resources)->CreateTexture(L"DiffuseLightTarget",
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM, _window.clientWidth, _window.clientHeight,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[1].target = GET_SINGLETON(Resources)->CreateTexture(L"SpecularLightTarget",
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM, _window.clientWidth, _window.clientHeight,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, dsTexture);
    }
}