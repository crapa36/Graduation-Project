#pragma once

class ImguiManager {
    DECLARE_SINGLETON(ImguiManager);
public:
    void Init(HWND hwnd, ID3D12Device* device, ID3D12DescriptorHeap* heap);
    void Update();
    void Render();
    void Release();
};
