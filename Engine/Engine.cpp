#include "pch.h"
#include "Engine.h"

void Engine::Init(const WindowInfo& info) {
    _window = info;
    ResizeWindow(info.width, info.height);

    // �׷��� ȭ�� ũ�⸦ ����
    _viewport = { 0.0f, 0.0f, static_cast<float>(info.width), static_cast<float>(info.height), 0.0f, 1.0f };
    _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    _device = make_shared<Device>();
    _cmdQueue = make_shared<CommandQueue>();
    _swapChain = make_shared<SwapChain>();
    _rootSignature = make_shared<RootSignature>();
    _cb = make_shared<ConstantBuffer>();
    _tableHeap = make_shared<TableDescriptorHeap>();

    _device->Init();
    _cmdQueue->Init(_device->GetDevice(), _swapChain);
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
    _rootSignature->Init(_device->GetDevice());
    _cb->Init(sizeof(Transform), 256);
    _tableHeap->Init(256);
}

void Engine::Render() {
    RenderBegin();

    // TODO : ������ ��ü �׸���

    RenderEnd();
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