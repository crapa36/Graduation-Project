#include "pch.h"
#include "TableDescriptorHeap.h"
#include "Engine.h"

// ************************
// GraphicsDescriptorHeap
// ************************

void GraphicsDescriptorHeap::Init(uint32 count) {
    _groupCount = count;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = count * CBV_SRV_REGISTER_COUNT;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    HRESULT hr = DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));
    if (FAILED(hr)) throw std::runtime_error("Failed to create descriptor heap.");

    _handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _groupSize = _handleSize * CBV_SRV_REGISTER_COUNT;
}

void GraphicsDescriptorHeap::Clear() {
    _currentGroupIndex = 0;
}

void GraphicsDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);
    DEVICE->CopyDescriptors(1, &destHandle, nullptr, 1, &srcHandle, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);
    DEVICE->CopyDescriptors(1, &destHandle, nullptr, 1, &srcHandle, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}


void GraphicsDescriptorHeap::SetSRVArray(D3D12_CPU_DESCRIPTOR_HANDLE* srcHandles, uint32 count, SRV_REGISTER startRegister) {
    // Get the starting CPU handle for the specified register
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(startRegister);

    // Create descriptor for the texture array
    for (uint32 i = 0; i < count; ++i) {
        DEVICE->CopyDescriptorsSimple(1, destHandle, srcHandles[i], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        destHandle.ptr += DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}


void GraphicsDescriptorHeap::CommitTable() {
    if (_currentGroupIndex >= _groupCount) {
        throw std::out_of_range("Current group index exceeds the number of descriptor groups.");
    }

    D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += _currentGroupIndex * _groupSize;
    GRAPHICS_CMD_LIST->SetGraphicsRootDescriptorTable(1, handle);

    _currentGroupIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCPUHandle(CBV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCPUHandle(SRV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCPUHandle(uint8 reg) {
    if (reg == 0) {
        throw std::invalid_argument("Register value must be greater than 0.");
    }
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += _currentGroupIndex * _groupSize;
    handle.ptr += (reg - 1) * _handleSize;
    return handle;
}


// ************************
// ComputeDescriptorHeap
// ************************

void ComputeDescriptorHeap::Init() {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = TOTAL_REGISTER_COUNT;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    HRESULT hr = DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));
    if (FAILED(hr)) throw std::runtime_error("Failed to create descriptor heap.");

    _handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);
    DEVICE->CopyDescriptors(1, &destHandle, nullptr, 1, &srcHandle, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);
    DEVICE->CopyDescriptors(1, &destHandle, nullptr, 1, &srcHandle, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}


void ComputeDescriptorHeap::SetSRVArray(D3D12_CPU_DESCRIPTOR_HANDLE* srcHandles, size_t count, SRV_REGISTER startRegister) {
    

    // Get the starting CPU handle for the specified register.
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(startRegister);

    // Update the SRV array in the compute descriptor heap.
    for (size_t i = 0; i < count; ++i) {
        // Copy the source SRV handle to the destination handle.
        DEVICE->CopyDescriptorsSimple(1, destHandle, srcHandles[i], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // Move to the next descriptor handle.
        destHandle.ptr += _handleSize;
    }

   
}

void ComputeDescriptorHeap::SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);
    DEVICE->CopyDescriptors(1, &destHandle, nullptr, 1, &srcHandle, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    // TODO : Add resource state changes if necessary.
}

void ComputeDescriptorHeap::CommitTable() {
    ID3D12DescriptorHeap* descHeap = _descHeap.Get();
    COMPUTE_CMD_LIST->SetDescriptorHeaps(1, &descHeap);

    D3D12_GPU_DESCRIPTOR_HANDLE handle = descHeap->GetGPUDescriptorHandleForHeapStart();
    COMPUTE_CMD_LIST->SetComputeRootDescriptorTable(0, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(CBV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(SRV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(UAV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(uint8 reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += reg * _handleSize;
    return handle;
}



// ************************
// ImguiDescriptorHeap
// ************************

void ImguiDescriptorHeap::Init() {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));
    if (FAILED(hr)) throw std::runtime_error("Failed to create descriptor heap.");

    _handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ImguiDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);
    DEVICE->CopyDescriptors(1, &destHandle, nullptr, 1, &srcHandle, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ImguiDescriptorHeap::CommitTable() {
    ID3D12DescriptorHeap* descHeap = _descHeap.Get();
    COMPUTE_CMD_LIST->SetDescriptorHeaps(1, &descHeap);

    D3D12_GPU_DESCRIPTOR_HANDLE handle = descHeap->GetGPUDescriptorHandleForHeapStart();
    COMPUTE_CMD_LIST->SetComputeRootDescriptorTable(0, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE ImguiDescriptorHeap::GetCPUHandle(CBV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ImguiDescriptorHeap::GetCPUHandle(SRV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ImguiDescriptorHeap::GetCPUHandle(UAV_REGISTER reg) {
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ImguiDescriptorHeap::GetCPUHandle(uint8 reg) {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += reg * _handleSize;
    return handle;
}
