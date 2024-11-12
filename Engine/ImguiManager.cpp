#include "pch.h"
#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"
#include "Engine.h"
#include "TableDescriptorHeap.h"

void ImguiManager::Init(HWND hwnd, ComPtr<ID3D12Device> device, ImguiDescriptorHeap idh) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = &ImGui::GetIO();
    _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    _io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // DPI 설정 확인 및 조정
    UINT dpi = GetDpiForWindow(hwnd);
    float scale = static_cast<float>(dpi) / 96.0f; // 기본 DPI는 96
    _io->DisplayFramebufferScale = ImVec2(scale, scale);

    ImGui::StyleColorsDark();

    // ImGui 스타일 스케일링
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);

    if (!ImGui_ImplWin32_Init(hwnd)) {
        throw std::runtime_error("Failed to initialize ImGui Win32 backend.");
    }

    if (!ImGui_ImplDX12_Init(
        device.Get(),
        2,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        idh.GetDescriptorHeap().Get(),
        idh.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        idh.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart())) {
        throw std::runtime_error("Failed to initialize ImGui DirectX12 backend.");
    }

    // 폰트 텍스처 생성
    _io->Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, _io->Fonts->GetGlyphRangesKorean());
    ImGui_ImplDX12_CreateDeviceObjects();
}

void ImguiManager::Update() {
    // Start the Dear ImGui frame
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window
    if (_show_demo_window)
        ImGui::ShowDemoWindow(&_show_demo_window);

    // 2. Show a simple window that we create ourselves
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");

        ImGui::Text("This is some useful text.");
        ImGui::Checkbox("Demo Window", &_show_demo_window);
        ImGui::Checkbox("Another Window", &_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&_clear_color);

        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _io->Framerate, _io->Framerate);
        ImGui::End();
    }

    // 3. Show another simple window
    if (_show_another_window) {
        ImGui::Begin("Another Window", &_show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            _show_another_window = false;
        ImGui::End();
    }

    ImGui::Render();
}

void ImguiManager::Render() {
    
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRAPHICS_CMD_LIST.Get());
}

void ImguiManager::Release() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}