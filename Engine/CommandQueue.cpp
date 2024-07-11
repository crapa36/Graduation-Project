#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

// *************************
// GraphicsCommandQueue
// *************************

// GraphicsCommandQueue Ŭ������ �Ҹ����Դϴ�.
// �潺 �̺�Ʈ �ڵ��� �ݽ��ϴ�.
GraphicsCommandQueue::~GraphicsCommandQueue() {
    ::CloseHandle(_fenceEvent);
}

// GraphicsCommandQueue Ŭ������ �ʱ�ȭ�մϴ�.
// DirectX 12 ����̽��� ���� ü���� ����Ͽ� ��� ť�� �ʱ�ȭ�մϴ�.
void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain) {
    _swapChain = swapChain;

    // ��� ť�� �����մϴ�.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

    // ��� �Ҵ��ڿ� ��� ����Ʈ�� �����մϴ�.
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    _cmdList->Close();

    // ���ҽ� ��� ����Ʈ�� ���� ��� �Ҵ��ڿ� ��� ����Ʈ�� �����մϴ�.
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));

    // CPU�� GPU�� ����ȭ �������� �潺�� �����մϴ�.
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
void GraphicsCommandQueue::WaitSync() {

    // �潺 ���� ������Ű��, ��� ť�� ��ȣ�� �����ϴ�.
    _fenceValue++;

    // GPU�� ��� ����� ó���� ������ ��ٸ��ϴ�.
    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue) {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

// �������� �����ϱ� ���� �ʿ��� �غ� �۾��� �����մϴ�.
void GraphicsCommandQueue::RenderBegin() {
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    int8 backIndex = _swapChain->GetBackBufferIndex();

    // ȭ�� ��� ���¿��� ������ Ÿ�� ���·� ���ҽ� ���¸� ��ȯ�մϴ�.
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTexture2D().Get(),
        D3D12_RESOURCE_STATE_PRESENT, // ȭ�� ��� ����
        D3D12_RESOURCE_STATE_RENDER_TARGET); // ������ Ÿ�� ����

    _cmdList->ResourceBarrier(1, &barrier);
}

// �������� ���� �� �ʿ��� ó���� �����մϴ�.
void GraphicsCommandQueue::RenderEnd() {
    int8 backIndex = _swapChain->GetBackBufferIndex();

    // ������ Ÿ�� ���¿��� ȭ�� ��� ���·� ���ҽ� ���¸� ��ȯ�մϴ�.
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTexture2D().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // ������ Ÿ�� ����
        D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ��� ����

    _cmdList->ResourceBarrier(1, &barrier);
    _cmdList->Close();

    // ��� ����Ʈ�� �����մϴ�.
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // ���� ü���� ���� ȭ�鿡 ����մϴ�.
    _swapChain->Present();

    // CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
    WaitSync();

    // ���� ü���� �ε����� ��ü�մϴ�.
    _swapChain->SwapIndex();
}

// ���ҽ� ��� ť�� ���ϴ�.
void GraphicsCommandQueue::FlushResourceCommandQueue() {
    _resCmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
    WaitSync();

    _resCmdAlloc->Reset();
    _resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}

// *************************
// ComputeCommandQueue
// *************************

// ComputeCommandQueue Ŭ������ �Ҹ����Դϴ�.
// �潺 �̺�Ʈ �ڵ��� �ݽ��ϴ�.
ComputeCommandQueue::~ComputeCommandQueue() {
    ::CloseHandle(_fenceEvent);
}

// ComputeCommandQueue Ŭ������ �ʱ�ȭ�մϴ�.
// DirectX 12 ����̽��� ����Ͽ� ��� ��� ť�� �ʱ�ȭ�մϴ�.
void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device) {
    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));

    // ��� �Ҵ��ڿ� ��� ����Ʈ�� �����մϴ�.
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

    // CPU�� GPU�� ����ȭ �������� �潺�� �����մϴ�.
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
void ComputeCommandQueue::WaitSync() {
    _fenceValue++;

    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue) {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

// ��� ��� ť�� ���ϴ�.
void ComputeCommandQueue::FlushComputeCommandQueue() {
    _cmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
    WaitSync();

    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);
}