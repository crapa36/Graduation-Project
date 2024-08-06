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
    bool _show_demo_window = true;
    bool _show_another_window = false;
    ImVec4 _clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO _io = {};
};