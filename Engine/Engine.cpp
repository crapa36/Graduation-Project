#include "pch.h"
#include "Engine.h"
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

void Engine::Init(const WindowInfo& window)
{
	_window = window;
	ResizeWindow(_window.width, -window.height);

	//그려질 화면의 크기 설정
	_viewport = { 0, 0, static_cast<FLOAT>(_window.width), static_cast<FLOAT>(_window.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, _window.width, _window.height);

	_device = make_shared<Device>();
	_commandQueue = make_shared<CommandQueue>();
	_swapChain = make_shared<SwapChain>();
	_descriptorHeap = make_shared<DescriptorHeap>();

	_device->Init();
	_commandQueue->Init(_device->GetDevice(), _swapChain, _descriptorHeap)
}

void Engine::Render()
{
}

void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;
	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(_window.hwnd, 0, 100, 100, _window.width, _window.height, 0);
}
