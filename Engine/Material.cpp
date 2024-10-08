#include "pch.h"
#include "Material.h"
#include "Engine.h"
#include "CubeMapTexture.h"

Material::Material() : Object(OBJECT_TYPE::MATERIAL) {
}

Material::~Material() {
}
void Material::PushGraphicsData() {
    // CBV upload (constant buffer for material parameters)
    CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&_params, sizeof(_params));

    // Collect SRV handles for all textures in the array
    vector<D3D12_CPU_DESCRIPTOR_HANDLE> textureHandles;
    for (size_t i = 0; i < _textures.size() && i < MAX_TEXTURES; ++i) {
        if (_textures[i] != nullptr) {
            textureHandles.push_back(_textures[i]->GetSRVHandle());
        }
       
    }

    // Upload the texture array to the descriptor heap (t0 to tMAX_TEXTURES-1)
    if (!textureHandles.empty()) {
        GEngine->GetGraphicsDescriptorHeap()->SetSRVArray(textureHandles.data(), textureHandles.size(), SRV_REGISTER::t0);
    }

    // Cube map texture (t6)
    if (_cubeMapTexture != nullptr) {
        GEngine->GetGraphicsDescriptorHeap()->SetSRV(_cubeMapTexture->GetSRVHandle(), SRV_REGISTER::t6);
    }

    // Update pipeline state
    _shader->Update();
}

void Material::PushComputeData() {
    // CBV upload (constant buffer for material parameters)
    CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&_params, sizeof(_params));

    // Collect SRV handles for all textures in the array
    vector<D3D12_CPU_DESCRIPTOR_HANDLE> textureHandles;
    for (size_t i = 0; i < _textures.size() && i < MAX_TEXTURES; ++i) {
        if (_textures[i] != nullptr) {
            textureHandles.push_back(_textures[i]->GetSRVHandle());
        }
       
    }

    // Upload the texture array to the compute descriptor heap
    if (!textureHandles.empty()) {
        GEngine->GetComputeDescriptorHeap()->SetSRVArray(textureHandles.data(), textureHandles.size(), SRV_REGISTER::t0);
    }

    // Cube map texture (t6)
    if (_cubeMapTexture != nullptr) {
        GEngine->GetComputeDescriptorHeap()->SetSRV(_cubeMapTexture->GetSRVHandle(), SRV_REGISTER::t6);
    }

    // Update pipeline state
    _shader->Update();
}



void Material::Dispatch(uint32 x, uint32 y, uint32 z) {

    // CBV + SRV + Set
    // State
    PushComputeData();

    // SetDescriptorHeaps + SetComputeRootDescriptorTable
    GEngine->GetComputeDescriptorHeap()->CommitTable();

    COMPUTE_CMD_LIST->Dispatch(x, y, z);

    GEngine->GetComputeCmdQueue()->FlushComputeCommandQueue();
}

shared_ptr<Material> Material::Clone() {
    shared_ptr<Material> material = make_shared<Material>();
    material->SetShader(_shader);
    material->_params = _params;
    material->_textures = _textures;

    return material;
}