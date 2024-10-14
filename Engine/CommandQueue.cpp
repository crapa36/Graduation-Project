#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"


// *************************
// GraphicsCommandQueue
// *************************

GraphicsCommandQueue::~GraphicsCommandQueue() {
    if (_fenceEvent) {
        ::CloseHandle(_fenceEvent);
    }
}

void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, std::shared_ptr<SwapChain> swapChain) {
    _swapChain = swapChain;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
    if (FAILED(hr)) throw std::runtime_error("Failed to create command queue.");

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
    if (FAILED(hr)) throw std::runtime_error("Failed to create command allocator.");

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    if (FAILED(hr)) throw std::runtime_error("Failed to create command list.");
    _cmdList->Close();

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
    if (FAILED(hr)) throw std::runtime_error("Failed to create resource command allocator.");

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));
    if (FAILED(hr)) throw std::runtime_error("Failed to create resource command list.");

    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    if (FAILED(hr)) throw std::runtime_error("Failed to create fence.");

    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!_fenceEvent) throw std::runtime_error("Failed to create fence event.");
}

void GraphicsCommandQueue::WaitSync() {
    _fenceValue++;

    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue) {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void GraphicsCommandQueue::RenderBegin() {
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    int8_t backIndex = _swapChain->GetBackBufferIndex();

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTexture2D().Get(),
        D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
        D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물

    _cmdList->SetGraphicsRootSignature(GRAPHICS_ROOT_SIGNATURE.Get());

    GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
    GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();
    GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TIME)->Clear();

    GEngine->GetGraphicsDescriptorHeap()->Clear();

    ID3D12DescriptorHeap* descHeap = GEngine->GetGraphicsDescriptorHeap()->GetDescriptorHeap().Get();
    _cmdList->SetDescriptorHeaps(1, &descHeap);

    _cmdList->ResourceBarrier(1, &barrier);
}

void GraphicsCommandQueue::RenderEnd() {
    int8_t backIndex = _swapChain->GetBackBufferIndex();

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTexture2D().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
        D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

    _cmdList->ResourceBarrier(1, &barrier);
    _cmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    _swapChain->Present(); // VSync 켜짐 (0으로 설정하면 VSync 꺼짐)

    WaitSync();

    _swapChain->SwapIndex();
}

void GraphicsCommandQueue::FlushResourceCommandQueue() {
    _resCmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    WaitSync();

    _resCmdAlloc->Reset();
    _resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}



// *************************
// ComputeCommandQueue
// *************************

ComputeCommandQueue::~ComputeCommandQueue() {
    if (_fenceEvent) {
        ::CloseHandle(_fenceEvent);
    }
}

void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device) {
    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    HRESULT hr = device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));
    if (FAILED(hr)) throw std::runtime_error("Failed to create compute command queue.");

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
    if (FAILED(hr)) throw std::runtime_error("Failed to create compute command allocator.");

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    if (FAILED(hr)) throw std::runtime_error("Failed to create compute command list.");

    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    if (FAILED(hr)) throw std::runtime_error("Failed to create fence.");

    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!_fenceEvent) throw std::runtime_error("Failed to create fence event.");
}

void ComputeCommandQueue::WaitSync() {
    _fenceValue++;

    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue) {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void ComputeCommandQueue::FlushComputeCommandQueue() {
    _cmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    WaitSync();

    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    COMPUTE_CMD_LIST->SetComputeRootSignature(COMPUTE_ROOT_SIGNATURE.Get());
}
