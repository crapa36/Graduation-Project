#include "pch.h"
#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"
#include "Engine.h"
#include "TableDescriptorHeap.h"

void ImguiManager::Init(HWND hwnd, ComPtr<ID3D12Device> device, ImguiDescriptorHeap idh) {
    // ImGui �ʱ�ȭ �� ���� üũ
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = ImGui::GetIO();
    (void)_io; // _io ���
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // DPI ���� Ȯ�� �� ����
    _io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); // �⺻�� ����, �ʿ�� ����

    // ��ũ �׸� ��Ÿ�� ����
    ImGui::StyleColorsDark();

    // ImGui Win32 �� DX12 �鿣�� �ʱ�ȭ
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
        throw std::runtime_error("Failed to initialize ImGui DX12 backend.");
    }
}
void ImguiManager::Update() {
    // ������ ����
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. ���� â ǥ�� (ImGui ���� ��� Ȯ�ο�)
    if (_show_demo_window) {
        ImGui::ShowDemoWindow(&_show_demo_window);
    }

    // 2. �⺻ "Hello, world!" â ���� �� ����� ���� ǥ��
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // â ����

        ImGui::Text("This is some useful text.");               // �ؽ�Ʈ ���
        ImGui::Checkbox("Demo Window", &_show_demo_window);     // ���� â ǥ�� ����
        ImGui::Checkbox("Another Window", &_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // �����̴� ����
        ImGui::ColorEdit3("clear color", &_clear_color.x);       // ���� ���� ���� (RGB�� ����)

        if (ImGui::Button("Button")) {                          // ��ư ����
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _io.Framerate, _io.Framerate);
        ImGui::End();
    }

    // 3. "Another Window" â ǥ��
    if (_show_another_window) {
        ImGui::Begin("Another Window", &_show_another_window);   // "Another Window" â ����
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
            _show_another_window = false;                       // â �ݱ� ��ư
        }
        ImGui::End();
    }
}
void ImguiManager::Render() {
    ImGui::Render();

    // DirectX 12 ��� ����Ʈ�� ���� ������ ����
    if (GRAPHICS_CMD_LIST) {
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRAPHICS_CMD_LIST.Get());
    }
    else {
        throw std::runtime_error("Graphics command list is not initialized.");
    }
}

void ImguiManager::Release() {
    // ImGui ���ҽ� ����
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
