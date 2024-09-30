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
    _io = ImGui::GetIO();
    (void)_io; // _io 사용
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // DPI 설정 확인 및 조정
    _io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); // 기본값 설정, 필요시 조정

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(
        device.Get(),
        2,
        DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
        idh.GetDescriptorHeap().Get(),
        idh.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        idh.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
    );
}

void ImguiManager::Update() {

    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (_show_demo_window)
        ImGui::ShowDemoWindow(&_show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &_show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&_clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _io.Framerate, _io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (_show_another_window) {
        ImGui::Begin("Another Window", &_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            _show_another_window = false;
        ImGui::End();
    }
}

void ImguiManager::Render() {
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRAPHICS_CMD_LIST.Get());
}

void ImguiManager::Release() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}