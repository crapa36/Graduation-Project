#include "pch.h"
#include "SwapChain.h"

// ���� ü�� �ʱ�ȭ
void SwapChain::Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue) {
    HRESULT hr = CreateSwapChain(info, dxgi, cmdQueue);
    if (FAILED(hr)) {
        throw std::runtime_error("���� ü�� ������ �����߽��ϴ�.");
    }
}

// ���� �������� ǥ��
void SwapChain::Present() {
    // �������� ǥ��
    HRESULT hr = _swapChain->Present(0, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("���� ü�� ǥ�� ����.");
    }
}

// �� ���� �ε��� ��ȯ
void SwapChain::SwapIndex() {
    _backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

// ���� ü�� ����
HRESULT SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue) {
    // ���� ���� ü�� ���� �ʱ�ȭ
    _swapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = static_cast<uint32>(info.width); // ������ �ʺ�
    sd.BufferDesc.Height = static_cast<uint32>(info.height); // ������ ����
    sd.BufferDesc.RefreshRate.Numerator = 60; // ȭ�� ���� ���� (����)
    sd.BufferDesc.RefreshRate.Denominator = 1; // ȭ�� ���� ���� (�и�)
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ������ ���÷��� ����
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1; // ��Ƽ ���ø� OFF
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �ĸ� ���ۿ� ������
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // ���� ���� (���� + �ĸ�)
    sd.OutputWindow = info.hwnd;
    sd.Windowed = info.windowed;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ��ü �� ���� ������ ���� ����
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    return dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);
}
