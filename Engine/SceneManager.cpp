#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Shader.h"
#include "Resources.h"

#include "GameObject.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Rigidbody.h"

#include "MeshData.h"

#include "TestScene.h"
#include "TitleScene.h"

void SceneManager::Init() {
#pragma region LayerMask
    SetLayerName(0, L"Default");
    SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"ComputeShader");

        // UAV �� Texture ����
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->CreateTexture(L"UAVTexture",
                                                                              DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
                                                                              CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
                                                                              D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"ComputeShader");
        material->SetShader(shader);
        material->SetInt(0, 1);
        GEngine->GetComputeDescriptorHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

        // ������ �׷� (1 * 1024 * 1)
        material->Dispatch(1, 1024, 1);
    }
#pragma endregion
    shared_ptr<TitleScene> titleScene = make_shared<TitleScene>();
    Scenes[L"TestMenuScene"] = titleScene->GetScene();

    shared_ptr<TestScene> testScene = make_shared<TestScene>();
    Scenes[L"TestScene"] = testScene->GetScene();
}

void SceneManager::Update() {
    if (_activeScene == nullptr)
        return;

    _activeScene->Update();
    _activeScene->LateUpdate();
    _activeScene->FinalUpdate();
}

// TEMP
void SceneManager::Render() {
    if (_activeScene)
        _activeScene->Render();
}

void SceneManager::SaveScene(wstring sceneName) {
    ofstream fout(sceneName, std::ios::binary);

    string str;
    fout.write(str.c_str(), str.size());

    for (auto& obj : _activeScene->GetGameObjects()) {
        fout << "<Object>" << endl;

        fout << "<Name>" << endl;
        fout << ws2s(obj->GetName()) << endl;

        if (obj->GetTransform()) {
            fout << "<Transform>" << endl;
            fout.write(reinterpret_cast<const char*>(&obj->GetTransform()->GetLocalPosition()), sizeof(Vec3));
            fout.write(reinterpret_cast<const char*>(&obj->GetTransform()->GetLocalRotation()), sizeof(Vec3));
            fout.write(reinterpret_cast<const char*>(&obj->GetTransform()->GetLocalScale()), sizeof(Vec3)) << endl;
        }

        if (obj->GetCamera()) {
            fout << "<Camera>" << endl;
            fout << obj->GetCamera()->GetNear() << endl;
            fout << obj->GetCamera()->GetFar() << endl;
            fout << obj->GetCamera()->GetFOV() << endl;
            fout << obj->GetCamera()->GetScale() << endl;
            fout << obj->GetCamera()->GetWidth() << endl;
            fout << obj->GetCamera()->GetHeight() << endl;
        }

        if (obj->GetMeshRenderer()) {
            fout << "<MeshRenderer>" << endl;

            fout << "<Mesh>" << endl;

            fout << "<Vertex>" << endl;
            uint32 _vertexCount = obj->GetMeshRenderer()->GetMesh()->GetVertexCount();
            uint32 bufferSize = _vertexCount * sizeof(Vertex);
            fout << _vertexCount << endl;

            Vertex* vertex = new Vertex[_vertexCount];

            void* vertexDataBuffer = nullptr;
            CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
            obj->GetMeshRenderer()->GetMesh()->GetVertexBuffer()->Map(0, &readRange, &vertexDataBuffer);
            ::memcpy(&vertex[0], vertexDataBuffer, bufferSize);
            obj->GetMeshRenderer()->GetMesh()->GetVertexBuffer()->Unmap(0, nullptr);

            for (int i = 0; i < _vertexCount; i++) {
                fout << vertex[i].pos.x << vertex[i].pos.y << vertex[i].pos.z << endl;
                fout << vertex[i].normal.x << vertex[i].normal.y << vertex[i].normal.z << endl;
            }

            fout << "</Vertex>" << endl;

            delete[] vertex;

            for (int i = 0; i < obj->GetMeshRenderer()->GetMesh()->GetIndexBuffer().size(); i++) {
                fout << "<Index>" << endl;

                uint32 _indexCount = obj->GetMeshRenderer()->GetMesh()->GetIndexBuffer().at(i).count;
                bufferSize = _indexCount * sizeof(uint32);
                fout << _indexCount << endl;

                uint32* index = new uint32[_indexCount];

                void* indexDataBuffer = nullptr;
                CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
                obj->GetMeshRenderer()->GetMesh()->GetIndexBuffer().at(i).buffer->Map(0, &readRange, &indexDataBuffer);
                ::memcpy(&index[0], indexDataBuffer, bufferSize);
                obj->GetMeshRenderer()->GetMesh()->GetIndexBuffer().at(i).buffer->Unmap(0, nullptr);
                for (int j = 0; j < _indexCount; j++) {
                    fout << index[j];
                }

                delete[] index;
            }
            fout << "</Mesh>" << endl;
            fout << "<Material>" << endl;
            shared_ptr<Material> material = obj->GetMeshRenderer()->GetMaterial();

            if (material->GetShader()->GetVsBlob()) {
                void* pVSData = material->GetShader()->GetVsBlob()->GetBufferPointer();
                size_t VSDataSize = material->GetShader()->GetVsBlob()->GetBufferSize();
                fout << VSDataSize;
                vector<char> vsData(VSDataSize);
                copy(static_cast<char*>(pVSData), static_cast<char*>(pVSData) + VSDataSize, vsData.begin());
                fout << vsData.size();
                for (int i = 0; i < vsData.size(); i++) {
                    fout << vsData.at(i) << endl;
                }
            }
            if (material->GetShader()->GetHsBlob()) {
                void* pVSData = material->GetShader()->GetHsBlob()->GetBufferPointer();
                size_t VSDataSize = material->GetShader()->GetHsBlob()->GetBufferSize();
                fout << VSDataSize;
                vector<char> vsData(VSDataSize);
                copy(static_cast<char*>(pVSData), static_cast<char*>(pVSData) + VSDataSize, vsData.begin());
                fout << vsData.size();
                for (int i = 0; i < vsData.size(); i++) {
                    fout << vsData.at(i) << endl;
                }
            }
            if (material->GetShader()->GetDsBlob()) {
                void* pVSData = material->GetShader()->GetDsBlob()->GetBufferPointer();
                size_t VSDataSize = material->GetShader()->GetDsBlob()->GetBufferSize();
                fout << VSDataSize;
                vector<char> vsData(VSDataSize);
                copy(static_cast<char*>(pVSData), static_cast<char*>(pVSData) + VSDataSize, vsData.begin());
                fout << vsData.size();
                for (int i = 0; i < vsData.size(); i++) {
                    fout << vsData.at(i) << endl;
                }
            }
            if (material->GetShader()->GetGsBlob()) {
                void* pVSData = material->GetShader()->GetGsBlob()->GetBufferPointer();
                size_t VSDataSize = material->GetShader()->GetGsBlob()->GetBufferSize();
                fout << VSDataSize;
                vector<char> vsData(VSDataSize);
                copy(static_cast<char*>(pVSData), static_cast<char*>(pVSData) + VSDataSize, vsData.begin());
                fout << vsData.size();
                for (int i = 0; i < vsData.size(); i++) {
                    fout << vsData.at(i) << endl;
                }
            }
            if (material->GetShader()->GetPsBlob()) {
                void* pVSData = material->GetShader()->GetPsBlob()->GetBufferPointer();
                size_t VSDataSize = material->GetShader()->GetPsBlob()->GetBufferSize();
                fout << VSDataSize;
                vector<char> vsData(VSDataSize);
                copy(static_cast<char*>(pVSData), static_cast<char*>(pVSData) + VSDataSize, vsData.begin());
                fout << vsData.size();
                for (int i = 0; i < vsData.size(); i++) {
                    fout << vsData.at(i) << endl;
                }
            }
            if (material->GetShader()->GetCsBlob()) {
                void* pVSData = material->GetShader()->GetCsBlob()->GetBufferPointer();
                size_t VSDataSize = material->GetShader()->GetCsBlob()->GetBufferSize();
                fout << VSDataSize;
                vector<char> vsData(VSDataSize);
                copy(static_cast<char*>(pVSData), static_cast<char*>(pVSData) + VSDataSize, vsData.begin());
                fout << vsData.size();
                for (int i = 0; i < vsData.size(); i++) {
                    fout << vsData.at(i) << endl;
                }
            }

            ShaderInfo shaderInfo = material->GetShader()->GetInfo();
            fout.write(reinterpret_cast<const char*>(&shaderInfo), sizeof(ShaderInfo));

            //Texture
            //fout << material->GetTextures().size();
            //for (int i = 0; i < material->GetTextures().size(); i++) {
            //        void* mappedData;
            //        void* binaryData = nullptr;
            //        shared_ptr<Texture> texture = material->GetTextures().at(i);
            //        uint64 dataSize = texture->GetWidth() * texture->GetHeight() * texture->GetTexture2D()->GetDesc().DepthOrArraySize * texture->GetTexture2D()->GetDesc().MipLevels * texture->GetTexture2D()->GetDesc().Format;
            //        fout << dataSize;
            //        material->GetTextures().at(i)->GetTexture2D()->Map(0, nullptr, &mappedData);
            //        memcpy(binaryData, mappedData, dataSize);
            //
            //
            //        material->GetTextures().at(i)->GetTexture2D()->Unmap(0, nullptr);
            //        fout.write(reinterpret_cast<const char*>(binaryData), dataSize);
            //}
            fout << "</MeshRenderer>" << endl;
        }

        /*      if (obj->GetLight()) {
                  fout << "<Light>" << endl;

                  fout.write(reinterpret_cast<const char*>(&obj->GetLight()->GetLightDirection()), sizeof(Vec3));
                  fout.write(reinterpret_cast<const char*>(&obj->GetLight()->GetDiffuse()), sizeof(Vec4));
                  fout.write(reinterpret_cast<const char*>(&obj->GetLight()->GetAmbient()), sizeof(Vec4));
                  fout.write(reinterpret_cast<const char*>(&obj->GetLight()->GetSpecular()), sizeof(Vec4));
                  fout << obj->GetLight()->GetLightRange() << endl;
                  fout << obj->GetLight()->GetLightAngle() << endl;
                  fout << obj->GetLight()->GetLightIndex() << endl;
              }*/

        if (obj->GetParticleSystem()) {
        }

        if (obj->GetTerrain()) {
            fout << "<Terrain>" << endl;
            fout << obj->GetTerrain()->GetsizeX() << endl;
            fout << obj->GetTerrain()->GetsizeZ() << endl;
        }

        if (obj->GetCollider()) {
        }

        if (obj->GetAnimator()) {
        }
    }

    fout << "</Scene>" << endl;
}

void SceneManager::LoadScene(wstring sceneName) {
    shared_ptr<Scene> scene = make_shared<Scene>();

    _activeScene = Scenes.at(sceneName);

    //SaveScene(L"../Resources/main_scene.bin");

    _activeScene->Awake();
    _activeScene->Start();
}

void SceneManager::ChangeScene(wstring sceneName) {
    _beforeScene = _activeScene;
    _activeScene = Scenes.at(sceneName);
}

void SceneManager::SetLayerName(uint8 index, const wstring& name) {
    const wstring& prevName = _layerNames[index];
    _layerIndex.erase(prevName);

    _layerNames[index] = name;
    _layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name) {
    auto findIt = _layerIndex.find(name);
    if (findIt == _layerIndex.end())
        return 0;

    return findIt->second;
}