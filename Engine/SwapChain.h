#pragma once

class SwapChain {
public:
    void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
    void Present();
    void SwapIndex();

    ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
    uint8 GetBackBufferIndex() { return _backBufferIndex; }

private:
    HRESULT CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

    ComPtr<IDXGISwapChain>	        _swapChain;
    uint32					        _backBufferIndex = 0;
};
