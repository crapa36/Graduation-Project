#include "pch.h"
#include "Texture.h"
#include "Engine.h"

Texture::Texture() : Object(OBJECT_TYPE::TEXTURE) {
}

Texture::~Texture() {
}

void Texture::Load(const wstring& path) {
    _path = path;

    // 파일 확장자 얻기
    wstring ext = fs::path(path).extension();

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(path.c_str(), nullptr, _image);
    else // png, jpg, jpeg, bmp
        ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    HRESULT hr = ::CreateTexture(DEVICE.Get(), _image.GetMetadata(), &_tex2D);
    if (FAILED(hr))
        assert(nullptr);
    _desc = _tex2D->GetDesc();
    vector<D3D12_SUBRESOURCE_DATA> subResources;

    hr = ::PrepareUpload(DEVICE.Get(),
                         _image.GetImages(),
                         _image.GetImageCount(),
                         _image.GetMetadata(),
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
    srvDesc.Format = _image.GetMetadata().format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;
    DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
}

void Texture::Create(DXGI_FORMAT format, uint32 width, uint32 height,
                     const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                     D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor) {
    _desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    _desc.Flags = resFlags;

    D3D12_CLEAR_VALUE optimizedClearValue = {};
    D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr;

    D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

    if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) {
        resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
        optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
        pOptimizedClearValue = &optimizedClearValue;
    }
    else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) {
        resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
        float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
        pOptimizedClearValue = &optimizedClearValue;
    }

    // Create Texture2D
    HRESULT hr = DEVICE->CreateCommittedResource(
        &heapProperty,
        heapFlags,
        &_desc,
        resourceStates,
        pOptimizedClearValue,
        IID_PPV_ARGS(&_tex2D));

    assert(SUCCEEDED(hr));

    CreateFromResource(_tex2D);
}

void Texture::CreateFromResource(ComPtr<ID3D12Resource> tex2D) {
    _tex2D = tex2D;

    _desc = tex2D->GetDesc();

    // 주요 조합
    // - DSV 단독 (조합X)
    // - SRV
    // - RTV + SRV
    if (_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) {

        // DSV
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.NumDescriptors = 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_dsvHeap));

        D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
        DEVICE->CreateDepthStencilView(_tex2D.Get(), nullptr, hDSVHandle);
    }
    else {
        if (_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) {

            // RTV
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.NumDescriptors = 1;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NodeMask = 0;
            DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeap));

            D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
            DEVICE->CreateRenderTargetView(_tex2D.Get(), nullptr, rtvHeapBegin);
        }

        if (_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) {

            // UAV
            D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
            uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            uavHeapDesc.NumDescriptors = 1;
            uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            uavHeapDesc.NodeMask = 0;
            DEVICE->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&_uavHeap));

            _uavHeapBegin = _uavHeap->GetCPUDescriptorHandleForHeapStart();

            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = _image.GetMetadata().format;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

            DEVICE->CreateUnorderedAccessView(_tex2D.Get(), nullptr, &uavDesc, _uavHeapBegin);
        }

        // SRV
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

        _srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = _image.GetMetadata().format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
    }
}

void Texture::CreateCubeMap(ID3D12Device* device, DXGI_FORMAT format, uint32_t width, uint32_t height,
                            const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                            D3D12_RESOURCE_FLAGS resFlags, DirectX::XMFLOAT4 clearColor) {
    // 큐브맵 텍스처 리소스 생성
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.DepthOrArraySize = 6; // 큐브맵은 6개의 면을 가짐
    texDesc.MipLevels = 1;
    texDesc.Format = format;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = resFlags;

    HRESULT hr = device->CreateCommittedResource(
        &heapProperty,
        heapFlags,
        &texDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&_tex2D)
    );

    if (FAILED(hr)) {
        // 오류 처리
        return;
    }

    // SRV 디스크립터 힙 생성
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));
    if (FAILED(hr)) {
        // 오류 처리
        return;
    }

    // SRV 생성
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = 1;
    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

    device->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeap->GetCPUDescriptorHandleForHeapStart());
}


void Texture::LoadCubeMapFromFiles(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const std::vector<std::wstring>& filePaths) {
    if (filePaths.size() != 6) {
        throw std::invalid_argument("Cube map requires exactly 6 texture files.");
    }

    // 큐브맵 텍스처 로드 로직 구현
    std::vector<DirectX::ScratchImage> images(6);
    for (size_t i = 0; i < 6; ++i) {
        HRESULT hr = DirectX::LoadFromWICFile(filePaths[i].c_str(), DirectX::WIC_FLAGS_NONE, nullptr, images[i]);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to load texture file.");
        }
    }

    // 텍스처 데이터를 큐브맵 텍스처에 업로드
    std::vector<D3D12_SUBRESOURCE_DATA> subresourceData(6);
    for (size_t i = 0; i < 6; ++i) {
        const DirectX::Image* img = images[i].GetImage(0, 0, 0);
        subresourceData[i].pData = img->pixels;
        subresourceData[i].RowPitch = img->rowPitch;
        subresourceData[i].SlicePitch = img->slicePitch;
    }

    // 업로드 힙 생성
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(_tex2D.Get(), 0, static_cast<UINT>(subresourceData.size()));

    Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadHeap;
    CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    HRESULT hr = device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create upload heap.");
    }

    // 텍스처 데이터를 업로드 힙에 복사
    UpdateSubresources(commandList, _tex2D.Get(), textureUploadHeap.Get(), 0, 0, static_cast<UINT>(subresourceData.size()), subresourceData.data());

    // 텍스처 상태를 복사 후 상태로 전환
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_tex2D.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);
}

Vec4 Texture::GetPixel(uint32 x, uint32 y) {
    Vec4 color = Vec4(0, 0, 0, 0);

    if (_image.GetPixels() == nullptr)
        return color;

    uint32 width = _image.GetMetadata().width;
    uint32 height = _image.GetMetadata().height;

    if (x < 0 || x >= width || y < 0 || y >= height)
        return color;

    uint8* pixels = _image.GetPixels();
    uint32 index = (y * width + x) * 4;

    color.x = pixels[index + 0] / 255.0f;
    color.y = pixels[index + 1] / 255.0f;
    color.z = pixels[index + 2] / 255.0f;
    color.w = pixels[index + 3] / 255.0f;

    return color;
}