#pragma once

#include "imgui.h" // ImGui 네임스페이스 포함

class ImguiManager {
    DECLARE_SINGLETON(ImguiManager);
public:
    void Init(HWND hwnd, ComPtr<ID3D12Device> device, class ImguiDescriptorHeap idh);
    void Update();
    void Render();
    void Release();

private:

    ImGuiIO _io = {};
};