#pragma once
#include "Object.h"

// �ؽ�ó Ŭ����: Object Ŭ������ ��ӹ���
class Texture : public Object {
public:
    Texture(); // ������
    virtual ~Texture(); // �Ҹ���

    virtual void Load(const wstring& path) override; // �ؽ�ó �ε� �Լ�
public:

    // �ؽ�ó ���� �Լ�
    void Create(DXGI_FORMAT format, uint32 width, uint32 height,
                const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    // ���ҽ��κ��� �ؽ�ó ���� �Լ�
    void CreateFromResource(ComPtr<ID3D12Resource> tex2D);
public:

    // Getter �Լ���
    ComPtr<ID3D12Resource> GetTexture2D() { return _tex2D; }
    ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }

private:
    ScratchImage			 		_image; // �̹��� ������
    ComPtr<ID3D12Resource>			_tex2D; // �ؽ�ó ���ҽ�

    // ��ũ���� ��
    ComPtr<ID3D12DescriptorHeap>	_srvHeap; // SRV��
    ComPtr<ID3D12DescriptorHeap>	_rtvHeap; // RTV��
    ComPtr<ID3D12DescriptorHeap>	_dsvHeap; // DSV��

private:
    D3D12_CPU_DESCRIPTOR_HANDLE		_srvHeapBegin = {}; // SRV ��ũ���� �ڵ�
};