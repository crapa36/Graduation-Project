#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

// *************************
// GraphicsCommandQueue
// *************************

// GraphicsCommandQueue 클래스의 소멸자입니다.
// 펜스 이벤트 핸들을 닫습니다.
GraphicsCommandQueue::~GraphicsCommandQueue() {
    ::CloseHandle(_fenceEvent);
}

// GraphicsCommandQueue 클래스를 초기화합니다.
// DirectX 12 디바이스와 스왑 체인을 사용하여 명령 큐를 초기화합니다.
void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain) {
    _swapChain = swapChain;

    // 명령 큐를 생성합니다.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

    // 명령 할당자와 명령 리스트를 생성합니다.
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    _cmdList->Close();

    // 리소스 명령 리스트를 위한 명령 할당자와 명령 리스트를 생성합니다.
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));

    // CPU와 GPU의 동기화 수단으로 펜스를 생성합니다.
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// CPU와 GPU의 동기화를 기다립니다.
void GraphicsCommandQueue::WaitSync() {

    // 펜스 값을 증가시키고, 명령 큐에 신호를 보냅니다.
    _fenceValue++;

    // GPU가 모든 명령을 처리할 때까지 기다립니다.
    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue) {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

// 렌더링을 시작하기 전에 필요한 준비 작업을 수행합니다.
void GraphicsCommandQueue::RenderBegin() {
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    int8 backIndex = _swapChain->GetBackBufferIndex();

    // 화면 출력 상태에서 렌더링 타겟 상태로 리소스 상태를 전환합니다.
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTexture2D().Get(),
        D3D12_RESOURCE_STATE_PRESENT, // 화면 출력 상태
        D3D12_RESOURCE_STATE_RENDER_TARGET); // 렌더링 타겟 상태

    _cmdList->ResourceBarrier(1, &barrier);
}

// 렌더링이 끝난 후 필요한 처리를 수행합니다.
void GraphicsCommandQueue::RenderEnd() {
    int8 backIndex = _swapChain->GetBackBufferIndex();

    // 렌더링 타겟 상태에서 화면 출력 상태로 리소스 상태를 전환합니다.
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTexture2D().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // 렌더링 타겟 상태
        D3D12_RESOURCE_STATE_PRESENT); // 화면 출력 상태

    _cmdList->ResourceBarrier(1, &barrier);
    _cmdList->Close();

    // 명령 리스트를 실행합니다.
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // 스왑 체인을 통해 화면에 출력합니다.
    _swapChain->Present();

    // CPU와 GPU의 동기화를 기다립니다.
    WaitSync();

    // 스왑 체인의 인덱스를 교체합니다.
    _swapChain->SwapIndex();
}

// 리소스 명령 큐를 비웁니다.
void GraphicsCommandQueue::FlushResourceCommandQueue() {
    _resCmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // CPU와 GPU의 동기화를 기다립니다.
    WaitSync();

    _resCmdAlloc->Reset();
    _resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}

// *************************
// ComputeCommandQueue
// *************************

// ComputeCommandQueue 클래스의 소멸자입니다.
// 펜스 이벤트 핸들을 닫습니다.
ComputeCommandQueue::~ComputeCommandQueue() {
    ::CloseHandle(_fenceEvent);
}

// ComputeCommandQueue 클래스를 초기화합니다.
// DirectX 12 디바이스를 사용하여 계산 명령 큐를 초기화합니다.
void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device) {
    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));

    // 명령 할당자와 명령 리스트를 생성합니다.
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

    // CPU와 GPU의 동기화 수단으로 펜스를 생성합니다.
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// CPU와 GPU의 동기화를 기다립니다.
void ComputeCommandQueue::WaitSync() {
    _fenceValue++;

    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue) {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

// 계산 명령 큐를 비웁니다.
void ComputeCommandQueue::FlushComputeCommandQueue() {
    _cmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // CPU와 GPU의 동기화를 기다립니다.
    WaitSync();

    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);
}