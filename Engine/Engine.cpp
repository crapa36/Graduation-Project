#include "pch.h"
#include "Engine.h"
#include "Material.h"
void Engine::Init(const WindowInfo& info) {
    _window = info;

    // 그려질 화면 크기를 설정
    _viewport = { 0.0f, 0.0f, static_cast<float>(info.width), static_cast<float>(info.height), 0.0f, 1.0f };
    _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    _device->Init();
    _cmdQueue->Init(_device->GetDevice(), _swapChain);
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
    _rootSignature->Init();
    _tableDescHeap->Init(256);
    _depthStencilBuffer->Init(_window);

    _input->Init(info.hwnd);
    _timer->Init();

    CreateConstantBuffer(CBV_REGISTER::b0, sizeof(Transform), 256);
    CreateConstantBuffer(CBV_REGISTER::b1, sizeof(MaterialParams), 256);

    ResizeWindow(info.width, info.height);
}

void Engine::Render() {
    RenderBegin();

    // TODO : 나머지 물체 그리기

    RenderEnd();
}

void Engine::Update() {
    _input->Update();
    _timer->Update();

    showFps();
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

    _depthStencilBuffer->Init(_window);
}

void Engine::showFps() {
    uint32 fps = _timer->GetFps();
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