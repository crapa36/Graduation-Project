#include "pch.h"
#include "Texture.h"
#include "Engine.h"

Texture::Texture() : Object(OBJECT_TYPE::TEXTURE) {
}

Texture::~Texture() {
}

// 텍스처 로드 함수 구현
void Texture::Load(const wstring& path) {

    // 파일 확장자 얻기
    wstring ext = fs::path(path).extension();

    // 확장자에 따라 적절한 로드 함수 호출
    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(path.c_str(), nullptr, _image);
    else // png, jpg, jpeg, bmp 등
        ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    // 텍스처 생성
    HRESULT hr = ::CreateTexture(DEVICE.Get(), _image.GetMetadata(), &_tex2D);
    if (FAILED(hr))
        assert(nullptr);

    // 업로드 준비
    vector<D3D12_SUBRESOURCE_DATA> subResources;
    hr = ::PrepareUpload(DEVICE.Get(),
                         _image.GetImages(),
                         _image.GetImageCount(),
                         _image.GetMetadata(),
                         subResources);

    if (FAILED(hr))
        assert(nullptr);

    // 업로드 버퍼 크기 계산 및 생성
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

    // 리소스 업데이트
    ::UpdateSubresources(RESOURCE_CMD_LIST.Get(),
                         _tex2D.Get(),
                         textureUploadHeap.Get(),
                         0, 0,
                         static_cast<unsigned int>(subResources.size()),
                         subResources.data());

    // 커맨드 큐를 통해 리소스 업데이트 반영
    GEngine->GetCmdQueue()->FlushResourceCommandQueue();

    // SRV 디스크립터 힙 생성 및 초기화
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

    _srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

    // SRV 생성
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = _image.GetMetadata().format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;
    DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
}

// 텍스처 생성 함수 구현
void Texture::Create(DXGI_FORMAT format, uint32 width, uint32 height,
                     const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                     D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor) {

    // 텍스처 리소스 설명 및 생성
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    desc.Flags = resFlags;

    // 최적화된 클리어 값 설정
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

    // 깊이 스텐실 또는 렌더 타겟 플래그에 따른 처리
    if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) {
        resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
        optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
    }
    else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) {
        resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
        float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
    }

    // 텍스처 리소스 생성
    HRESULT hr = DEVICE->CreateCommittedResource(
        &heapProperty,
        heapFlags,
        &desc,
        resourceStates,
        &optimizedClearValue,
        IID_PPV_ARGS(&_tex2D));

    assert(SUCCEEDED(hr));

    // 생성된 리소스로부터 텍스처 생성
    CreateFromResource(_tex2D);
}

// 리소스로부터 텍스처 생성 함수 구현
void Texture::CreateFromResource(ComPtr<ID3D12Resource> tex2D) {
    _tex2D = tex2D;

    // 리소스 설명 가져오기
    D3D12_RESOURCE_DESC desc = tex2D->GetDesc();

    // 플래그에 따른 디스크립터 힙 생성 및 뷰 생성
    if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) {

        // DSV 생성
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.NumDescriptors = 1;
        DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_dsvHeap));

        D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
        DEVICE->CreateDepthStencilView(_tex2D.Get(), nullptr, hDSVHandle);
    }
    else {
        if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) {

            // RTV 생성
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.NumDescriptors = 1;
            DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeap));

            D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
            DEVICE->CreateRenderTargetView(_tex2D.Get(), nullptr, rtvHeapBegin);
        }

        // SRV 생성
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

        _srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
    }
}