#include "pch.h"
#include "DebugLineManager.h"
#include "Engine.h"
#include "TableDescriptorHeap.h"
#include "Shader.h"
#include "Resources.h"

// Add Line
void DebugLineManager::AddLine(const Vec3& start, const Vec3& end, const Vec4& color) {
    LineVertex startVertex = { start, color };
    LineVertex endVertex = { end, color };
    _lineVertices.push_back(startVertex);
    _lineVertices.push_back(endVertex);
}

// Clear stored lines
void DebugLineManager::Clear() {
    _lineVertices.clear();
}

// Render Debug Lines
void DebugLineManager::Render() {
    if (_lineVertices.empty()) return;
    _shader = GET_SINGLETON(Resources)->Get<Shader>(L"DebugLine");

    // Initialize Graphics Descriptor Heap for Debug Line Data
    GEngine->GetGraphicsDescriptorHeap()->Clear();

    // Update Shader Pipeline State for Debug Lines
    _shader->Update();

    // Set up descriptor heap with line data
    D3D12_CPU_DESCRIPTOR_HANDLE lineDataHandle = CreateLineDataSRV();
    GEngine->GetGraphicsDescriptorHeap()->SetSRV(lineDataHandle, SRV_REGISTER::t0);

    // Commit SRV Table for Graphics Pipeline
    GEngine->GetGraphicsDescriptorHeap()->CommitTable();

    // Draw Line List
    GRAPHICS_CMD_LIST->IASetPrimitiveTopology(_shader->GetInfo().primitiveTopology);
    GRAPHICS_CMD_LIST->DrawInstanced(_lineVertices.size(), 1, 0, 0);
}

// Create Shader Resource View (SRV) for Line Data
D3D12_CPU_DESCRIPTOR_HANDLE DebugLineManager::CreateLineDataSRV() {
    // Create buffer to store line vertices
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(LineVertex) * _lineVertices.size());
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    ComPtr<ID3D12Resource> vertexBuffer;

    HRESULT hr = DEVICE->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&vertexBuffer));

    if (FAILED(hr)) throw std::runtime_error("Failed to create vertex buffer for DebugLineManager.");

    // Map vertex data
    void* mappedData;
    CD3DX12_RANGE readRange(0, 0);
    vertexBuffer->Map(0, &readRange, &mappedData);
    memcpy(mappedData, _lineVertices.data(), sizeof(LineVertex) * _lineVertices.size());
    vertexBuffer->Unmap(0, nullptr);

    // Create SRV for vertex buffer
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = GEngine->GetGraphicsDescriptorHeap()->GetCPUHandle(SRV_REGISTER::t0);
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = static_cast<UINT>(_lineVertices.size());
    srvDesc.Buffer.StructureByteStride = sizeof(LineVertex);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    DEVICE->CreateShaderResourceView(vertexBuffer.Get(), &srvDesc, srvHandle);

    return srvHandle;
}
