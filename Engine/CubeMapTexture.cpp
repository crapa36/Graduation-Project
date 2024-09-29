// CubeMapTexture.cpp
#include "pch.h"
#include "CubeMapTexture.h"
#include "Engine.h"

CubeMapTexture::CubeMapTexture() : Texture() {
}

CubeMapTexture::~CubeMapTexture() {
}

void CubeMapTexture::Load(const wstring& path) {
    _path = path;

    // ���� Ȯ���� ���
    wstring ext = fs::path(path).extension();
    std::unique_ptr<uint8_t[]> ddsData;
    vector<D3D12_SUBRESOURCE_DATA> subResources;
    DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
    bool bIsCubeMap = true;

    // Ȯ���ڰ� .dds �Ǵ� .DDS�� �� LoadDDSTextureFromFileEx �Լ� ���
    if (ext == L".dds" || ext == L".DDS") {
        HRESULT hr = ::LoadDDSTextureFromFileEx(
            DEVICE.Get(),
            path.c_str(),
            0,
            D3D12_RESOURCE_FLAG_NONE,
            DDS_LOADER_DEFAULT,
            &_tex2D,
            ddsData,
            subResources,
            &ddsAlphaMode,
            &bIsCubeMap
        );
        if (FAILED(hr)) {
            assert(nullptr);
        }
    }
    else if (ext == L".tga" || ext == L".TGA") {
        ::LoadFromTGAFile(path.c_str(), nullptr, _image);
    }
    else { // png, jpg, jpeg, bmp ��
        ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _image);
    }

    // ť��� �ؽ��ķ� ��ȯ�� ��� ó��
    if (bIsCubeMap) {
        _desc = _tex2D->GetDesc();

        // ���ε� ���ҽ� �غ�
        const uint64 bufferSize = ::GetRequiredIntermediateSize(_tex2D.Get(), 0, static_cast<uint32>(subResources.size()));

        D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        ComPtr<ID3D12Resource> textureUploadHeap;
        HRESULT hr = DEVICE->CreateCommittedResource(
            &heapProperty,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(textureUploadHeap.GetAddressOf())
        );
        if (FAILED(hr)) {
            assert(nullptr);
        }

        // ���� ���ҽ� ������Ʈ
        ::UpdateSubresources(RESOURCE_CMD_LIST.Get(),
                             _tex2D.Get(),
                             textureUploadHeap.Get(),
                             0, 0,
                             static_cast<unsigned int>(subResources.size()),
                             subResources.data());

        GEngine->GetGraphicsCmdQueue()->FlushResourceCommandQueue();

        // Shader Resource View ����
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

        _srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = _tex2D->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.TextureCube.MipLevels = 1;
        DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
    }
}