#pragma once

#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

class SwapChain;
class DescriptorHeap;

// GraphicsCommandQueue 클래스는 DirectX 12의 명령 큐를 관리합니다.
// 이 클래스는 그래픽 관련 명령을 큐에 넣고 실행하는 기능을 제공합니다.
class GraphicsCommandQueue {
public:
    ~GraphicsCommandQueue();

    // 초기화 함수: DirectX 12 디바이스와 스왑 체인을 사용하여 명령 큐를 초기화합니다.
    void Init(ComPtr<ID3D12Device> device, std::shared_ptr<SwapChain> swapChain);

    // 동기화 함수: CPU와 GPU의 동기화를 기다립니다.
    void WaitSync();

    // 렌더링 시작 전 준비 작업을 수행합니다.
    void RenderBegin();

    // 렌더링이 끝난 후 필요한 처리를 수행합니다.
    void RenderEnd();

    // 리소스 명령 큐를 비웁니다.
    void FlushResourceCommandQueue();

    // 명령 큐, 명령 리스트, 리소스 명령 리스트에 대한 접근자 함수들입니다.
    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return _cmdList; }
    ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }
private:

    // DirectX 12 명령 큐, 명령 할당자, 그래픽 명령 리스트, 리소스 명령 리스트
    ComPtr<ID3D12CommandQueue>			_cmdQueue;
    ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_cmdList;

    ComPtr<ID3D12CommandAllocator>		_resCmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

    // 동기화를 위한 펜스, 펜스 값, 펜스 이벤트
    ComPtr<ID3D12Fence>					_fence;
    uint32								_fenceValue = 0;
    HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;

    std::shared_ptr<SwapChain>		_swapChain;
};

// ComputeCommandQueue 클래스는 DirectX 12의 계산 명령 큐를 관리합니다.
// 이 클래스는 계산 관련 명령을 큐에 넣고 실행하는 기능을 제공합니다.
class ComputeCommandQueue {
public:
    ~ComputeCommandQueue();

    // 초기화 함수: DirectX 12 디바이스를 사용하여 명령 큐를 초기화합니다.
    void Init(ComPtr<ID3D12Device> device);

    // 동기화 함수: CPU와 GPU의 동기화를 기다립니다.
    void WaitSync();

    // 계산 명령 큐를 비웁니다.
    void FlushComputeCommandQueue();

    // 명령 큐와 계산 명령 리스트에 대한 접근자 함수들입니다.
    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:

    // DirectX 12 명령 큐, 명령 할당자, 계산 명령 리스트
    ComPtr<ID3D12CommandQueue>			_cmdQueue;
    ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_cmdList;

    // 동기화를 위한 펜스, 펜스 값, 펜스 이벤트
    ComPtr<ID3D12Fence>					_fence;
    uint32								_fenceValue = 0;
    HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};
