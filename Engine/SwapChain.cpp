#include "pch.h"
#include "SwapChain.h"

// 스왑 체인 초기화
void SwapChain::Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue) {
    HRESULT hr = CreateSwapChain(info, dxgi, cmdQueue);
    if (FAILED(hr)) {
        throw std::runtime_error("스왑 체인 생성에 실패했습니다.");
    }
}

// 현재 프레임을 표시
void SwapChain::Present() {
    // 프레임을 표시
    HRESULT hr = _swapChain->Present(0, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("스왑 체인 표시 실패.");
    }
}

// 백 버퍼 인덱스 교환
void SwapChain::SwapIndex() {
    _backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

// 스왑 체인 생성
HRESULT SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue) {
    // 이전 스왑 체인 정보 초기화
    _swapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 너비
    sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 높이
    sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율 (분자)
    sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율 (분모)
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 버퍼 개수 (전면 + 후면)
    sd.OutputWindow = info.hwnd;
    sd.Windowed = info.windowed;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 교체 시 이전 프레임 정보 버림
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    return dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);
}
