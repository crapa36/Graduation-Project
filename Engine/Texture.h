#pragma once
#include "Object.h"

class Texture : public Object {
public:
    Texture();
    virtual ~Texture();

    virtual void Load(const wstring& path) override;

public:
    void Create(DXGI_FORMAT format, uint32 width, uint32 height,
                const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

public:
    ComPtr<ID3D12Resource> GetTexture2D() { return _tex2D; }
    ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }
    wstring GetPath() { return _path; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() { return _uavHeapBegin; }

    FLOAT GetWidth() { return static_cast<FLOAT>(_desc.Width); }
    FLOAT GetHeight() { return static_cast<FLOAT>(_desc.Height); }

    Vec4 GetPixel(uint32 x, uint32 y);

private:
    wstring                         _path = L"";

    ScratchImage			 		_image;
    D3D12_RESOURCE_DESC             _desc;
    ComPtr<ID3D12Resource>			_tex2D;

    ComPtr<ID3D12DescriptorHeap>	_srvHeap;
    ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
    ComPtr<ID3D12DescriptorHeap>	_dsvHeap;
    ComPtr<ID3D12DescriptorHeap>	_uavHeap;

private:
    D3D12_CPU_DESCRIPTOR_HANDLE		_srvHeapBegin = {};
    D3D12_CPU_DESCRIPTOR_HANDLE		_uavHeapBegin = {};
};
