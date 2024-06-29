#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

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

    CreateConstantBuffer(CBV_REGISTER::b0, sizeof(TransformParams), 256);
    CreateConstantBuffer(CBV_REGISTER::b1, sizeof(MaterialParams), 256);

    ResizeWindow(info.width, info.height);

    GET_SINGLETON(Input)->Init(info.hwnd);
    GET_SINGLETON(Timer)->Init();
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

    // TODO : 나머지 물체 그리기
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

    _depthStencilBuffer->Init(_window);
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