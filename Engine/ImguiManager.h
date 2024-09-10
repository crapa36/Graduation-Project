#pragma once

#include "imgui.h" // ImGui 네임스페이스 포함
#include <wrl/client.h> // For ComPtr
using Microsoft::WRL::ComPtr; // Assuming you are using ComPtr from WRL

class ImguiDescriptorHeap; // Forward declaration for ImguiDescriptorHeap

class ImguiManager {
    DECLARE_SINGLETON(ImguiManager); // Ensure this macro is properly defined
public:
    void Init(HWND hwnd, ComPtr<ID3D12Device> device, ImguiDescriptorHeap idh);
    void Update();
    void Render();
    void Release();

private:
    bool _show_demo_window = true;
    bool _show_another_window = false;
    ImVec4 _clear_color{ 0.45f, 0.55f, 0.60f, 1.00f }; // Modern brace initialization
    ImGuiIO _io{}; // Initialize _io using modern C++ syntax
};
