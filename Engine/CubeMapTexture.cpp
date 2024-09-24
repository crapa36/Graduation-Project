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

    // 파일 확장자 얻기
    wstring ext = fs::path(path).extension();

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(path.c_str(), nullptr, _image);
    else // png, jpg, jpeg, bmp
        ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    // 전개도 이미지를 큐브맵 텍스쳐로 변환
    ScratchImage cubeImage;
    HRESULT hr = cubeImage.InitializeCubeFromImages(_image.GetImages(), _image.GetImageCount());
    if (FAILED(hr))
        assert(nullptr);

    hr = ::CreateTexture(DEVICE.Get(), cubeImage.GetMetadata(), &_tex2D);
    if (FAILED(hr))
        assert(nullptr);
    _desc = _tex2D->GetDesc();
    vector<D3D12_SUBRESOURCE_DATA> subResources;

    hr = ::PrepareUpload(DEVICE.Get(),
                         cubeImage.GetImages(),
                         cubeImage.GetImageCount(),
                         cubeImage.GetMetadata(),
                         subResources);

    if (FAILED(hr))
        assert(nullptr);

    const uint64 bufferSize = ::GetRequiredIntermediateSize(_tex2D.Get(), 0, static_cast<uint32>(subResources.size()));

    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    ComPtr<ID3D12Resource> textureUploadHeap;
    hr = DEVICE->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

    if (FAILED(hr))
        assert(nullptr);

    ::UpdateSubresources(RESOURCE_CMD_LIST.Get(),
                         _tex2D.Get(),
                         textureUploadHeap.Get(),
                         0, 0,
                         static_cast<unsigned int>(subResources.size()),
                         subResources.data());

    GEngine->GetGraphicsCmdQueue()->FlushResourceCommandQueue();

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

    _srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = cubeImage.GetMetadata().format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.TextureCube.MipLevels = 1;
    DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
}