#pragma once
#include "Object.h"


class Texture : public Object {
public:
    Texture();
    virtual ~Texture();

    // �ؽ�ó �ε� �޼��� (���� ��θ� ���� �ؽ�ó �ε�)
    virtual void Load(const std::wstring& path) override;

public:
    // �ؽ�ó ���� �޼��� (���� ����)
    void Create(DXGI_FORMAT format, uint32 width, uint32 height,
                const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    // ���ҽ��� ������� �ؽ�ó ����
    void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

    void LoadCubeMap(const std::vector<std::wstring>& paths);

   
public:
    // Getter �Լ���
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


    // �ȼ� �����͸� ��ȯ�ϴ� �޼���
    Vec4 GetPixel(uint32 x, uint32 y);

protected:
    // ��� ������
    std::wstring                    _path = L"";                 // �ؽ�ó ���� ���
    DirectX::ScratchImage            _image;                     // DirectXTex �̹��� ������
    D3D12_RESOURCE_DESC              _desc;                      // �ؽ�ó ����
    ComPtr<ID3D12Resource>           _tex2D;                     // �ؽ�ó ���ҽ�

    ComPtr<ID3D12DescriptorHeap>     _srvHeap;                   // Shader Resource View ��
    ComPtr<ID3D12DescriptorHeap>     _rtvHeap;                   // Render Target View ��
    ComPtr<ID3D12DescriptorHeap>     _dsvHeap;                   // Depth Stencil View ��
    ComPtr<ID3D12DescriptorHeap>     _uavHeap;                   // Unordered Access View ��

protected:
    // �� �ڵ�
    D3D12_CPU_DESCRIPTOR_HANDLE      _srvHeapBegin = {};
    D3D12_CPU_DESCRIPTOR_HANDLE      _uavHeapBegin = {};
};
