#include "pch.h"
#include "StructuredBuffer.h"
#include "Engine.h"
#include <stdexcept> // std::runtime_error

StructuredBuffer::StructuredBuffer() {
}

StructuredBuffer::~StructuredBuffer() {
}

void StructuredBuffer::Init(uint32 elementSize, uint32 elementCount, void* initialData) {
    _elementSize = elementSize;
    _elementCount = elementCount;
    _resourceState = D3D12_RESOURCE_STATE_COMMON;

    // Buffer
    {
        uint64 bufferSize = static_cast<uint64>(_elementSize) * _elementCount;
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        HRESULT hr = DEVICE->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            _resourceState,
            nullptr,
            IID_PPV_ARGS(&_buffer));
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create committed resource for StructuredBuffer.");
        }

        if (initialData) {
            CopyInitialData(bufferSize, initialData);
        }
    }

    // SRV
    {
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        HRESULT hr = DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create SRV descriptor heap for StructuredBuffer.");
        }

        _srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = _elementCount;
        srvDesc.Buffer.StructureByteStride = _elementSize;
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        DEVICE->CreateShaderResourceView(_buffer.Get(), &srvDesc, _srvHeapBegin);
    }

    // UAV
    {
        D3D12_DESCRIPTOR_HEAP_DESC uavheapDesc = {};
        uavheapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        uavheapDesc.NumDescriptors = 1;
        uavheapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        HRESULT hr = DEVICE->CreateDescriptorHeap(&uavheapDesc, IID_PPV_ARGS(&_uavHeap));
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UAV descriptor heap for StructuredBuffer.");
        }

        _uavHeapBegin = _uavHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = _elementCount;
        uavDesc.Buffer.StructureByteStride = _elementSize;
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        DEVICE->CreateUnorderedAccessView(_buffer.Get(), nullptr, &uavDesc, _uavHeapBegin);
    }
}

void StructuredBuffer::PushGraphicsData(SRV_REGISTER reg) {
    GEngine->GetGraphicsDescriptorHeap()->SetSRV(_srvHeapBegin, reg);
}

void StructuredBuffer::PushComputeSRVData(SRV_REGISTER reg) {
    GEngine->GetComputeDescriptorHeap()->SetSRV(_srvHeapBegin, reg);
}

void StructuredBuffer::PushComputeUAVData(UAV_REGISTER reg) {
    GEngine->GetComputeDescriptorHeap()->SetUAV(_uavHeapBegin, reg);
}

void StructuredBuffer::CopyInitialData(uint64 bufferSize, void* initialData) {
    ComPtr<ID3D12Resource> readBuffer = nullptr;
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);
    D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    HRESULT hr = DEVICE->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&readBuffer));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create committed resource for initial data upload.");
    }

    uint8* dataBegin = nullptr;
    D3D12_RANGE readRange{ 0, 0 };
    hr = readBuffer->Map(0, &readRange, reinterpret_cast<void**>(&dataBegin));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to map read buffer for initial data upload.");
    }
    memcpy(dataBegin, initialData, bufferSize);
    readBuffer->Unmap(0, nullptr);

    // Common -> Copy
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_buffer.Get(),
                                                                              D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

        RESOURCE_CMD_LIST->ResourceBarrier(1, &barrier);
    }

    RESOURCE_CMD_LIST->CopyBufferRegion(_buffer.Get(), 0, readBuffer.Get(), 0, bufferSize);

    // Copy -> Common
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_buffer.Get(),
                                                                              D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
        RESOURCE_CMD_LIST->ResourceBarrier(1, &barrier);
    }

    GEngine->GetGraphicsCmdQueue()->FlushResourceCommandQueue();

    _resourceState = D3D12_RESOURCE_STATE_COMMON;
}
