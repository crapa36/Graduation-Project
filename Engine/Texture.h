#pragma once
#include "Object.h"

// 텍스처 클래스: Object 클래스를 상속받음
class Texture : public Object {
public:
    Texture(); // 생성자
    virtual ~Texture(); // 소멸자

    virtual void Load(const wstring& path) override; // 텍스처 로드 함수
public:

    // 텍스처 생성 함수
    void Create(DXGI_FORMAT format, uint32 width, uint32 height,
                const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    // 리소스로부터 텍스처 생성 함수
    void CreateFromResource(ComPtr<ID3D12Resource> tex2D);
public:

    // Getter 함수들
    ComPtr<ID3D12Resource> GetTexture2D() { return _tex2D; }
    ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }

private:
    ScratchImage			 		_image; // 이미지 데이터
    ComPtr<ID3D12Resource>			_tex2D; // 텍스처 리소스

    // 디스크립터 힙
    ComPtr<ID3D12DescriptorHeap>	_srvHeap; // SRV용
    ComPtr<ID3D12DescriptorHeap>	_rtvHeap; // RTV용
    ComPtr<ID3D12DescriptorHeap>	_dsvHeap; // DSV용

private:
    D3D12_CPU_DESCRIPTOR_HANDLE		_srvHeapBegin = {}; // SRV 디스크립터 핸들
};