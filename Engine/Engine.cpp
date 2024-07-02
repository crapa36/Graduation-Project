#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"

void Engine::Init(const WindowInfo& info) {
    _window = info;

    // �׷��� ȭ�� ũ�⸦ ����
    _viewport = { 0.0f, 0.0f, static_cast<float>(info.width), static_cast<float>(info.height), 0.0f, 1.0f };
    _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    _device->Init();
    _cmdQueue->Init(_device->GetDevice(), _swapChain);
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
    _rootSignature->Init();
    _tableDescHeap->Init(256);

    CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 256);
    CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256);
    CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);

    CreateRenderTargetGroups();

    ResizeWindow(info.width, info.height);

    GET_SINGLETON(Input)->Init(info.hwnd);
    GET_SINGLETON(Timer)->Init();
    GET_SINGLETON(Resources)->Init();
}

void Engine::Update() {
    GET_SINGLETON(Input)->Update();
    GET_SINGLETON(Timer)->Update();
    GET_SINGLETON(SceneManager)->Update();
    Render();

    showFps();
}

void Engine::Render() {
    RenderBegin();

    // TODO : ������ ��ü �׸���
    GET_SINGLETON(SceneManager)->Render();

    RenderEnd();
}

void Engine::LateUpdate() {

    // TODO
}

void Engine::RenderBegin() {
    _cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::RenderEnd() {
    _cmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height) {
    _window.width = width;
    _window.height = height;
    RECT rect = { 0,0,width,height };
    ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    ::SetWindowPos(_window.hwnd, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
}

void Engine::showFps() {
    uint32 fps = GET_SINGLETON(Timer)->GetFps();
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
    shared_ptr<Texture> dsTexture =
        GET_SINGLETON(Resources)->CreateTexture(L"DepthStencil",
                                                DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
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

    // Deferred Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

        rtVec[0].target = GET_SINGLETON(Resources)->CreateTexture(L"PositionTarget",
                                                                  DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[1].target = GET_SINGLETON(Resources)->CreateTexture(L"NormalTarget",
                                                                  DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[2].target = GET_SINGLETON(Resources)->CreateTexture(L"DiffuseTarget",
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
                                                                  CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                                                  D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
        _renderTargetGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
    }
}