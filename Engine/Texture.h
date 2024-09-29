#pragma once
#include "Object.h"


class Texture : public Object {
public:
    Texture();
    virtual ~Texture();

    // 텍스처 로드 메서드 (파일 경로를 통한 텍스처 로드)
    virtual void Load(const std::wstring& path) override;

public:
    // 텍스처 생성 메서드 (직접 생성)
    void Create(DXGI_FORMAT format, uint32 width, uint32 height,
                const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    // 리소스를 기반으로 텍스처 생성
    void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

    void LoadCubeMap(const std::vector<std::wstring>& paths);

   
public:
    // Getter 함수들
    ComPtr<ID3D12Resource> GetTexture2D() { return _tex2D; }
    ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }
    std::wstring GetPath() { return _path; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() { return _uavHeapBegin; }

    FLOAT GetWidth() { return static_cast<FLOAT>(_desc.Width); }
    FLOAT GetHeight() { return static_cast<FLOAT>(_desc.Height); }


    // 픽셀 데이터를 반환하는 메서드
    Vec4 GetPixel(uint32 x, uint32 y);

protected:
    // 멤버 변수들
    std::wstring                    _path = L"";                 // 텍스처 파일 경로
    DirectX::ScratchImage            _image;                     // DirectXTex 이미지 데이터
    D3D12_RESOURCE_DESC              _desc;                      // 텍스처 설명
    ComPtr<ID3D12Resource>           _tex2D;                     // 텍스처 리소스

    ComPtr<ID3D12DescriptorHeap>     _srvHeap;                   // Shader Resource View 힙
    ComPtr<ID3D12DescriptorHeap>     _rtvHeap;                   // Render Target View 힙
    ComPtr<ID3D12DescriptorHeap>     _dsvHeap;                   // Depth Stencil View 힙
    ComPtr<ID3D12DescriptorHeap>     _uavHeap;                   // Unordered Access View 힙

protected:
    // 힙 핸들
    D3D12_CPU_DESCRIPTOR_HANDLE      _srvHeapBegin = {};
    D3D12_CPU_DESCRIPTOR_HANDLE      _uavHeapBegin = {};
};
