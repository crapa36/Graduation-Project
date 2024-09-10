#include "pch.h"
#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"
#include "Engine.h"
#include "TableDescriptorHeap.h"

void ImguiManager::Init(HWND hwnd, ComPtr<ID3D12Device> device, ImguiDescriptorHeap idh) {
    // ImGui 초기화 및 버전 체크
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = ImGui::GetIO();
    (void)_io; // _io 사용
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // DPI 설정 확인 및 조정
    _io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); // 기본값 설정, 필요시 조정

    // 다크 테마 스타일 설정
    ImGui::StyleColorsDark();

    // ImGui Win32 및 DX12 백엔드 초기화
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
    // 프레임 시작
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. 데모 창 표시 (ImGui 샘플 기능 확인용)
    if (_show_demo_window) {
        ImGui::ShowDemoWindow(&_show_demo_window);
    }

    // 2. 기본 "Hello, world!" 창 생성 및 사용자 위젯 표시
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // 창 시작

        ImGui::Text("This is some useful text.");               // 텍스트 출력
        ImGui::Checkbox("Demo Window", &_show_demo_window);     // 데모 창 표시 여부
        ImGui::Checkbox("Another Window", &_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // 슬라이더 위젯
        ImGui::ColorEdit3("clear color", &_clear_color.x);       // 색상 편집 위젯 (RGB만 전달)

        if (ImGui::Button("Button")) {                          // 버튼 생성
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _io.Framerate, _io.Framerate);
        ImGui::End();
    }

    // 3. "Another Window" 창 표시
    if (_show_another_window) {
        ImGui::Begin("Another Window", &_show_another_window);   // "Another Window" 창 시작
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
            _show_another_window = false;                       // 창 닫기 버튼
        }
        ImGui::End();
    }
}
void ImguiManager::Render() {
    ImGui::Render();

    // DirectX 12 명령 리스트에 렌더 데이터 제출
    if (GRAPHICS_CMD_LIST) {
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRAPHICS_CMD_LIST.Get());
    }
    else {
        throw std::runtime_error("Graphics command list is not initialized.");
    }
}

void ImguiManager::Release() {
    // ImGui 리소스 해제
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
