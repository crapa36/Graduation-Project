#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Shader.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"

#include "TestCameraScript.h"
#include "TestPlayerScript.h"
#include "TestDragonScript.h"

#include "Resources.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Rigidbody.h"

#include "MeshData.h"

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

    shared_ptr<Scene> scene = make_shared<Scene>();

    _activeScene = LoadTestScene();

    //SaveScene(L"../Resources/main_scene.bin");

    _activeScene->Awake();
    _activeScene->Start();
}

void SceneManager::ChangeScene(wstring sceneName) {
    //_beforeScene = _activeScene;
    //_activeScene = Scenes.at(sceneName);
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

shared_ptr<Scene> SceneManager::LoadTestScene() {
    shared_ptr<Scene> scene = make_shared<Scene>();
#pragma region FBX
    {
        shared_ptr<MeshData> meshData = GET_SINGLETON(Resources)->LoadFBX(L"..\\Resources\\FBX\\Dragon.fbx");

        vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

        for (auto& gameObject : gameObjects) {
            gameObject->SetName(L"Dragon");
            gameObject->SetCheckFrustum(false);
            gameObject->AddComponent(make_shared<BoxCollider>());
            gameObject->AddComponent(make_shared<TestDragonScript>());
            gameObject->AddComponent(make_shared<Rigidbody>());
            dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider())->SetExtents(Vec3(40.f, 30.f, 40.f));
            dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider())->SetCenter(Vec3(0.f, 10.f, 0.f));
            gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, 0.f));
            gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
            gameObject->GetRigidbody()->SetUseGravity(true);
            gameObject->GetRigidbody()->SetElasticity(0.0f);

            gameObject->SetStatic(false);
            scene->AddGameObject(gameObject);
        }
#pragma region Camera
        {
            shared_ptr<GameObject> camera = make_shared<GameObject>();
            camera->SetName(L"Main_Camera");
            camera->AddComponent(make_shared<Transform>());
            camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
            camera->AddComponent(make_shared<TestCameraScript>());
            camera->GetCamera()->SetFar(10000.f); // Far 10000 ����
            camera->GetTransform()->SetLocalPosition(Vec3(0.f, 70.f, -200.f));

            camera->GetTransform()->SetParent(gameObjects.front()->GetTransform());
            camera->GetTransform()->SetInheritRotation(false);
            camera->GetTransform()->SetInheritScale(false);

            uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
            camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
            scene->AddGameObject(camera);

            //#pragma region Player
            //        {
            //            shared_ptr<GameObject> player = make_shared<GameObject>();
            //            player->SetName(L"Player");
            //            player->AddComponent(make_shared<Transform>());
            //
            //            player->AddComponent(make_shared<BoxCollider>());
            //
            //            player->GetTransform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));
            //
            //            player->AddComponent(make_shared<TestPlayerScript>());
            //
            //            player->GetTransform()->SetLocalPosition(Vec3(0.f, -50.f, 200.f));
            //            player->SetStatic(false);
            //            shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
            //            {
            //                shared_ptr<Mesh> playerMesh = GET_SINGLETON(Resources)->LoadCubeMesh();
            //                meshRenderer->SetMesh(playerMesh);
            //            }
            //            {
            //                shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Wood");
            //                meshRenderer->SetMaterial(material->Clone());
            //            }
            //            player->GetTransform()->SetParent(camera->GetTransform());
            //
            //            player->GetTransform()->SetInheritRotation(false);
            //            player->GetTransform()->SetInheritScale(false);
            //
            //            player->GetCollider()->SetExtents(Vec3(50.f, 50.f, 50.f));
            //
            //            player->AddComponent(meshRenderer);
            //            scene->AddGameObject(player);
            //        }
            //#pragma endregion
        }
#pragma endregion
    }
#pragma endregion

#pragma region UI_Camera
    {
        shared_ptr<GameObject> camera = make_shared<GameObject>();
        camera->SetName(L"Orthographic_Camera");
        camera->AddComponent(make_shared<Transform>());
        camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
        camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
        uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
        camera->GetCamera()->SetCullingMaskAll(); // �� ����
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI�� ����
        scene->AddGameObject(camera);
    }
#pragma endregion

#pragma region SkyBox
    {
        shared_ptr<GameObject> skybox = make_shared<GameObject>();
        skybox->SetName(L"SkyBox");
        skybox->AddComponent(make_shared<Transform>());
        skybox->SetCheckFrustum(false);
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadSphereMesh();
            meshRenderer->SetMesh(sphereMesh);
        }
        {
            shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Skybox");
            shared_ptr<Texture> texture = GET_SINGLETON(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\Sky_01.jpg");
            shared_ptr<Material> material = make_shared<Material>();
            material->SetShader(shader);
            material->SetTexture(0, texture);
            meshRenderer->SetMaterial(material);
        }
        skybox->AddComponent(meshRenderer);
        scene->AddGameObject(skybox);
    }
#pragma endregion

#pragma region Object
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->SetName(L"SphereObj");
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<SphereCollider>());
        obj->AddComponent(make_shared<Rigidbody>());
        obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
        obj->GetTransform()->SetLocalPosition(Vec3(200, 2000.f, 50.f));

        obj->SetStatic(false);
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadSphereMesh();
            meshRenderer->SetMesh(sphereMesh);
        }
        {
            shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Pebbles");
            meshRenderer->SetMaterial(material->Clone());
        }
        obj->AddComponent(meshRenderer);

        obj->GetCollider()->SetRadius(100.f);

        obj->GetRigidbody()->SetUseGravity(true);
        scene->AddGameObject(obj);
    }
#pragma endregion

#pragma region Terrain
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<Terrain>());
        obj->AddComponent(make_shared<BoxCollider>());
        obj->AddComponent(make_shared<MeshRenderer>());

        obj->GetTransform()->SetLocalScale(Vec3(50.f, 400.f, 50.f));
        obj->GetTransform()->SetLocalPosition(Vec3(-1600.f, -200.f, -1600.f));
        obj->SetStatic(true);
        obj->GetTerrain()->Init(64, 64);
        dynamic_pointer_cast<BoxCollider>(obj->GetCollider())->SetCenter(Vec3(1600.f, 0.f, 1600.f));
        dynamic_pointer_cast<BoxCollider>(obj->GetCollider())->SetExtents(Vec3(3200.f, 1.f, 3200.f));

        obj->SetCheckFrustum(false);

        scene->AddGameObject(obj);
    }
#pragma endregion

#pragma region UI_Test
    for (int32 i = 0; i < 6; i++) {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->SetLayerIndex(GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI")); // UI
        obj->AddComponent(make_shared<Transform>());
        obj->GetTransform()->SetLocalScale(Vec3(200.f, 200.f, 200.f));
        obj->GetTransform()->SetLocalPosition(Vec3(-700.f + (i * 260), 350.f, 500.f));
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> mesh = GET_SINGLETON(Resources)->LoadRectangleMesh();
            meshRenderer->SetMesh(mesh);
        }
        {
            shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Texture");

            shared_ptr<Texture> texture;
            if (i < 3)
                texture = GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
            else if (i < 5)
                texture = GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
            else
                texture = GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

            shared_ptr<Material> material = make_shared<Material>();
            material->SetShader(shader);
            material->SetTexture(0, texture);
            meshRenderer->SetMaterial(material);
        }
        obj->AddComponent(meshRenderer);
        scene->AddGameObject(obj);
    }
#pragma endregion

#pragma region Directional Light
    {
        shared_ptr<GameObject> light = make_shared<GameObject>();
        light->AddComponent(make_shared<Transform>());

        light->AddComponent(make_shared<Light>());
        light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 1.f));
        light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
        light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
        light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
        light->GetLight()->SetSpecular(Vec3(0.2f, 0.2f, 0.2f));
        /*shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> frustumMesh = GET_SINGLETON(Resources)->LoadCameraFrustumMesh(light->GetLight()->GetShadowCamera()->GetCamera());
            meshRenderer->SetMesh(frustumMesh);
        }
        {
            shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Frustum");
            meshRenderer->SetMaterial(material->Clone());
        }
        light->AddComponent(meshRenderer);*/
        scene->AddGameObject(light);
    }
#pragma endregion

#pragma region Point Light
    {
        shared_ptr<GameObject> light = make_shared<GameObject>();
        light->AddComponent(make_shared<Transform>());
        light->GetTransform()->SetLocalPosition(Vec3(100.f, 50.f, 0.f));
        light->AddComponent(make_shared<Light>());

        //light->GetLight()->SetLightDirection(Vec3(-1.f, -1.f, 0));
        light->GetLight()->SetLightType(LIGHT_TYPE::POINT);
        light->GetLight()->SetDiffuse(Vec3(1.0f, 0.f, 0.0f));
        light->GetLight()->SetAmbient(Vec3(0.3f, 0.0f, 0.0f));
        light->GetLight()->SetSpecular(Vec3(0.3f, 0.0f, 0.0f));
        light->GetLight()->SetLightRange(200.f);

        scene->AddGameObject(light);
    }
#pragma endregion

#pragma region Spot Light
    {
        shared_ptr<GameObject> light = make_shared<GameObject>();
        light->AddComponent(make_shared<Transform>());
        light->GetTransform()->SetLocalPosition(Vec3(300.f, 0.f, 400.f));
        light->AddComponent(make_shared<Light>());
        light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));
        light->GetLight()->SetLightType(LIGHT_TYPE::SPOT);
        light->GetLight()->SetDiffuse(Vec3(0.0f, 0.f, 1.f));
        light->GetLight()->SetAmbient(Vec3(0.0f, 0.0f, 0.1f));
        light->GetLight()->SetSpecular(Vec3(0.0f, 0.0f, 0.1f));
        light->GetLight()->SetLightRange(1000.f);
        light->GetLight()->SetLightAngle(3.14f / 4);

        scene->AddGameObject(light);
    }
#pragma endregion

#pragma region ParticleSystem
    {
        shared_ptr<GameObject> particle = make_shared<GameObject>();
        particle->SetName(L"ParticleSystem");
        particle->AddComponent(make_shared<Transform>());
        particle->AddComponent(make_shared<ParticleSystem>());
        particle->SetCheckFrustum(false);
        particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, -200.f));
        scene->AddGameObject(particle);
    }
#pragma endregion

    //#pragma region BIN
    //    {
    //        wstring path = L"../Resources/BIN/Apache.bin";
    //        shared_ptr<MeshData> meshData = GET_SINGLETON(Resources)->LoadBIN(path);
    //
    //        vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
    //        for (auto& gameObject : gameObjects) {
    //            gameObject->SetName(L"Apache");
    //            gameObject->SetCheckFrustum(false);
    //            scene->AddGameObject(gameObject);
    //            gameObject->SetStatic(false);
    //        }
    //    }
    //#pragma endregion

    return scene;
}