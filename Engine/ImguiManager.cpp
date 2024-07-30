#include "pch.h"
#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"
#include "Engine.h"

void ImguiManager::Init(HWND hwnd, ID3D12Device* device, ID3D12DescriptorHeap* heap) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(
        device,
        2,
        DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
        m_renderingResources.GetImguiDescriptorHeap().GetDescriptorHeap().Get(),
        m_renderingResources.GetImguiDescriptorHeap().GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        m_renderingResources.GetImguiDescriptorHeap().GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
    );
    ImGui_ImplDX12_Init(device, 2, DXGI_FORMAT_R8G8B8A8_UNORM, heap->GetCPUDescriptorHandleForHeapStart(), heap->GetGPUDescriptorHandleForHeapStart());
}

void ImguiManager::Update() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void ImguiManager::Render() {
    ID3D12GraphicsCommandList* pCommandList = GEngine->GetGraphicsCommandList();
    m_renderingResources.GetImguiDescriptorHeap().GetDescriptorHeap()->SetName(L"Imgui Descriptor Heap");

    pCommandList->SetDescriptorHeaps(1, m_renderingResources.GetImguiDescriptorHeap().GetDescriptorHeap().GetAddressOf());
    D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle = m_renderingResources.GetRTVDescriptorHeap().GetRTVCPUHandle(m_renderingResources.GetFrameIndex());
    pCommandList->OMSetRenderTargets(1u, &rtvCPUHandle, FALSE, nullptr);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
}

void ImguiManager::Release() {
}