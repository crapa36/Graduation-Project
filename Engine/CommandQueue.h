#pragma once

#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

class SwapChain;
class DescriptorHeap;

// GraphicsCommandQueue Ŭ������ DirectX 12�� ��� ť�� �����մϴ�.
// �� Ŭ������ �׷��� ���� ����� ť�� �ְ� �����ϴ� ����� �����մϴ�.
class GraphicsCommandQueue {
public:
    ~GraphicsCommandQueue();

    // �ʱ�ȭ �Լ�: DirectX 12 ����̽��� ���� ü���� ����Ͽ� ��� ť�� �ʱ�ȭ�մϴ�.
    void Init(ComPtr<ID3D12Device> device, std::shared_ptr<SwapChain> swapChain);

    // ����ȭ �Լ�: CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
    void WaitSync();

    // ������ ���� �� �غ� �۾��� �����մϴ�.
    void RenderBegin();

    // �������� ���� �� �ʿ��� ó���� �����մϴ�.
    void RenderEnd();

    // ���ҽ� ��� ť�� ���ϴ�.
    void FlushResourceCommandQueue();

    // ��� ť, ��� ����Ʈ, ���ҽ� ��� ����Ʈ�� ���� ������ �Լ����Դϴ�.
    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return _cmdList; }
    ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }
private:

    // DirectX 12 ��� ť, ��� �Ҵ���, �׷��� ��� ����Ʈ, ���ҽ� ��� ����Ʈ
    ComPtr<ID3D12CommandQueue>			_cmdQueue;
    ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_cmdList;

    ComPtr<ID3D12CommandAllocator>		_resCmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

    // ����ȭ�� ���� �潺, �潺 ��, �潺 �̺�Ʈ
    ComPtr<ID3D12Fence>					_fence;
    uint32								_fenceValue = 0;
    HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;

    std::shared_ptr<SwapChain>		_swapChain;
};

// ComputeCommandQueue Ŭ������ DirectX 12�� ��� ��� ť�� �����մϴ�.
// �� Ŭ������ ��� ���� ����� ť�� �ְ� �����ϴ� ����� �����մϴ�.
class ComputeCommandQueue {
public:
    ~ComputeCommandQueue();

    // �ʱ�ȭ �Լ�: DirectX 12 ����̽��� ����Ͽ� ��� ť�� �ʱ�ȭ�մϴ�.
    void Init(ComPtr<ID3D12Device> device);

    // ����ȭ �Լ�: CPU�� GPU�� ����ȭ�� ��ٸ��ϴ�.
    void WaitSync();

    // ��� ��� ť�� ���ϴ�.
    void FlushComputeCommandQueue();

    // ��� ť�� ��� ��� ����Ʈ�� ���� ������ �Լ����Դϴ�.
    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:

    // DirectX 12 ��� ť, ��� �Ҵ���, ��� ��� ����Ʈ
    ComPtr<ID3D12CommandQueue>			_cmdQueue;
    ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_cmdList;

    // ����ȭ�� ���� �潺, �潺 ��, �潺 �̺�Ʈ
    ComPtr<ID3D12Fence>					_fence;
    uint32								_fenceValue = 0;
    HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};
