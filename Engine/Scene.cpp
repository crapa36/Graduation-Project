#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "MeshRenderer.h"
#include "PhysicsManager.h"
#include "Timer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"
#include "BaseCollider.h"

void Scene::Awake() {
    for (const shared_ptr<GameObject>& gameObject : _gameObjects) {
        gameObject->Awake();
    }
}

void Scene::Start() {
    for (const shared_ptr<GameObject>& gameObject : _gameObjects) {
        gameObject->Start();
    }
}

void Scene::Update() {
    for (const shared_ptr<GameObject>& gameObject : _gameObjects) {
        gameObject->Update();
    }
    GET_SINGLETON(PhysicsManager)->Update();
}

void Scene::LateUpdate() {
    for (const shared_ptr<GameObject>& gameObject : _gameObjects) {
        gameObject->LateUpdate();
    }
    GET_SINGLETON(PhysicsManager)->FinalUpdate();
}
void Scene::FinalUpdate() {
    for (const shared_ptr<GameObject>& gameObject : _gameObjects) {
        gameObject->FinalUpdate();
    }
    GET_SINGLETON(PhysicsManager)->FinalUpdate();
}

void Scene::Render() {
    PushLightData();

    ClearRTV();

    RenderShadow();

    RenderReflection();

    RenderDeferred();
    

    RenderLights();
    
    RenderFinal();

    RenderForward();
}

void Scene::ClearRTV() {

    // SwapChain Group 초기화
    int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

    // Shadow Group 초기화
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();

    // Deferred Group 초기화
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

    // Lighting Group 초기화
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();

    // Reflection Group 초기화
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::REFLECTION)->ClearRenderTargetView();
}

void Scene::RenderShadow() {
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

    for (auto& light : _lights) {
        if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL)
            continue;

        light->RenderShadow();
    }

    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void Scene::RenderDeferred() {

    // Deferred OMSet
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

    shared_ptr<Camera> mainCamera = _cameras[0];
    mainCamera->SortGameObject();
    mainCamera->Render_Deferred();

    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void Scene::RenderReflection() {
    shared_ptr<Camera> reflectionCamera = GetReflectionCamera();
    if (reflectionCamera) {
        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::REFLECTION)->OMSetRenderTargets();

        reflectionCamera->SortGameObject();
        reflectionCamera->Render_Deferred();

        GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::REFLECTION)->WaitTargetToResource();
    }
}

void Scene::RenderLights() {
    shared_ptr<Camera> mainCamera = _cameras[0];
    Camera::S_MatView = mainCamera->GetViewMatrix();
    Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

    // 광원을 그린다.
    for (auto& light : _lights) {
        light->Render();
    }

    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void Scene::RenderFinal() {

    // Swapchain OMSet
    int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
    GEngine->GetRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

    GET_SINGLETON(Resources)->Get<Material>(L"Final")->PushGraphicsData();
    GET_SINGLETON(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::RenderForward() {


    shared_ptr<Camera> mainCamera = _cameras[0];
    shared_ptr<Camera> reflectionCamera = GetReflectionCamera();
    mainCamera->Render_Forward();
    for (auto& camera : _cameras) {
        if (camera == mainCamera)
            continue;
        if (camera == reflectionCamera)
            continue;

        camera->SortGameObject();
        camera->Render_Forward();
    }
}

shared_ptr<Camera> Scene::GetMainCamera() {
    if (_cameras.empty())
        return nullptr;

    return _cameras[0];
}

shared_ptr<class Camera> Scene::GetReflectionCamera()
{
    for (shared_ptr<GameObject> obj : _gameObjects) {
        if (obj->GetName() == L"Reflection_Camera") {
            return obj->GetCamera();
        }
    }

    return nullptr;
}

void Scene::PushLightData() {
    LightParams lightParams = {};

    for (auto& light : _lights) {
        const LightInfo& lightInfo = light->GetLightInfo();

        light->SetLightIndex(lightParams.lightCount);

        lightParams.lights[lightParams.lightCount] = lightInfo;
        lightParams.lightCount++;
    }

    CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject) {
    if (gameObject->GetCamera() != nullptr) {
        _cameras.push_back(gameObject->GetCamera());
    }
    else if (gameObject->GetLight() != nullptr) {
        _lights.push_back(gameObject->GetLight());
    }

    _gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject) {
    if (gameObject->GetCamera()) {
        auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
        if (findIt != _cameras.end())
            _cameras.erase(findIt);
    }
    else if (gameObject->GetLight()) {
        auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
        if (findIt != _lights.end())
            _lights.erase(findIt);
    }

    auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
    if (findIt != _gameObjects.end())
        _gameObjects.erase(findIt);
}

void Scene::SaveScene(wstring path) {
}

void Scene::LoadScene(wstring path) {
    string str;

    ifstream in(path, std::ios::binary);

    while (!in.eof()) {
        getline(in, str);
        if (str == "<Object>") {
            while (1) {
                shared_ptr<GameObject> obj = make_shared<GameObject>();
                getline(in, str);
                if (str == "<Name>") {
                    string name;
                    in >> name;
                    obj->SetName(s2ws(name));
                }
                if (str == "<Transform>") {
                    shared_ptr<Transform> transform = make_shared<Transform>();
                    XMFLOAT3 position;
                    in >> position.x >> position.y >> position.z;
                    transform->SetLocalPosition(position);
                    in >> position.x >> position.y >> position.z;
                    transform->SetLocalRotation(position);
                    in >> position.x >> position.y >> position.z;
                    transform->SetLocalScale(position);
                    obj->AddComponent(transform);
                }
                if (str == "<Camera>") {
                    shared_ptr<Camera> camera = make_shared<Camera>();
                    float n;
                    in >> n;
                    camera->SetNear(n);
                    in >> n;
                    camera->SetFar(n);
                    in >> n;
                    camera->SetFOV(n);
                    in >> n;
                    camera->SetScale(n);
                    in >> n;
                    camera->SetWidth(n);
                    in >> n;
                    camera->SetHeight(n);
                    obj->AddComponent(camera);
                }
                if (str == "<MeshRenderer>") {
                    shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

                    shared_ptr<Mesh> mesh = make_shared<Mesh>();
                    shared_ptr<Material> material = make_shared<Material>();

                    vector<Vertex> vertexes;
                    vector<vector<uint32>> indexes;
                    getline(in, str);
                    if (str == "<Mesh>") {
                        while (1) {
                            getline(in, str);
                            if (str == "<Vertex>") {
                                uint32 count;
                                in >> count;

                                Vertex vertex;
                                XMFLOAT3 position;
                                XMFLOAT3 normal;
                                for (int i = 0; i < count; i++) {
                                    in >> position.x >> position.y >> position.z;
                                    in >> normal.x >> normal.y >> normal.z;
                                    vertex.pos = position;
                                    vertex.normal = normal;
                                    vertexes.push_back(vertex);
                                }
                            }
                            if (str == "<Index>") {
                                vector<uint32> index;
                                uint32 indexCount, _index;
                                in >> indexCount;

                                for (int i = 0; i < indexCount; i++) {
                                    in >> _index;
                                    index.push_back(_index);
                                }
                                indexes.push_back(index);
                            }
                            if (str == "</Mesh>") {
                                mesh->Create(vertexes, indexes);
                                break;
                            }
                        }
                    }
                    if (str == "<Material>") {
                        vector<char> Data;
                        int size;
                        size_t DataSize;

                        //Shader
                        {
                            in >> DataSize;
                            in >> size;
                            for (int i = 0; i < size; i++) {
                                Data.push_back(NULL);
                                in >> Data.back();
                            }
                            ComPtr<ID3DBlob> pBlob;
                            HRESULT hr = D3DCreateBlob(DataSize, &pBlob);
                            if (SUCCEEDED(hr)) {
                                memcpy(pBlob->GetBufferPointer(), Data.data(), DataSize);
                            }
                            material->GetShader()->SetVsBlob(pBlob);
                        }
                        {
                            in >> DataSize;
                            in >> size;
                            for (int i = 0; i < size; i++) {
                                Data.push_back(NULL);
                                in >> Data.back();
                            }
                            ComPtr<ID3DBlob> pBlob;
                            HRESULT hr = D3DCreateBlob(DataSize, &pBlob);
                            if (SUCCEEDED(hr)) {
                                memcpy(pBlob->GetBufferPointer(), Data.data(), DataSize);
                            }
                            material->GetShader()->SetHsBlob(pBlob);
                        }
                        {
                            in >> DataSize;
                            in >> size;
                            for (int i = 0; i < size; i++) {
                                Data.push_back(NULL);
                                in >> Data.back();
                            }
                            ComPtr<ID3DBlob> pBlob;
                            HRESULT hr = D3DCreateBlob(DataSize, &pBlob);
                            if (SUCCEEDED(hr)) {
                                memcpy(pBlob->GetBufferPointer(), Data.data(), DataSize);
                            }
                            material->GetShader()->SetDsBlob(pBlob);
                        }
                        {
                            in >> DataSize;
                            in >> size;
                            for (int i = 0; i < size; i++) {
                                Data.push_back(NULL);
                                in >> Data.back();
                            }
                            ComPtr<ID3DBlob> pBlob;
                            HRESULT hr = D3DCreateBlob(DataSize, &pBlob);
                            if (SUCCEEDED(hr)) {
                                memcpy(pBlob->GetBufferPointer(), Data.data(), DataSize);
                            }
                            material->GetShader()->SetGsBlob(pBlob);
                        }
                        {
                            in >> DataSize;
                            in >> size;
                            for (int i = 0; i < size; i++) {
                                Data.push_back(NULL);
                                in >> Data.back();
                            }
                            ComPtr<ID3DBlob> pBlob;
                            HRESULT hr = D3DCreateBlob(DataSize, &pBlob);
                            if (SUCCEEDED(hr)) {
                                memcpy(pBlob->GetBufferPointer(), Data.data(), DataSize);
                            }
                            material->GetShader()->SetPsBlob(pBlob);
                        }
                        {
                            in >> DataSize;
                            in >> size;
                            for (int i = 0; i < size; i++) {
                                Data.push_back(NULL);
                                in >> Data.back();
                            }
                            ComPtr<ID3DBlob> pBlob;
                            HRESULT hr = D3DCreateBlob(DataSize, &pBlob);
                            if (SUCCEEDED(hr)) {
                                memcpy(pBlob->GetBufferPointer(), Data.data(), DataSize);
                            }
                            material->GetShader()->SetCsBlob(pBlob);
                        }

                        ShaderInfo shaderInfo;
                        in.read(reinterpret_cast<char*>(&shaderInfo), sizeof(shaderInfo));

                        material->GetShader()->CreateGraphicsShader(shaderInfo);
                        material->GetShader()->CreateComputeShader();

                        //Texture
                        in >> size;
                        string path;
                        for (int i = 0; i < size; i++) {
                        }
                    }
                    if (str == "</MeshRenderer>") {
                        break;
                    }
                    meshRenderer->SetMesh(mesh);
                    meshRenderer->SetMaterial(material);
                }
                if (str == "<Light>") {
                    shared_ptr<Light> light = make_shared<Light>();
                    XMFLOAT3 lightDirection;
                    XMFLOAT4 diffuse, ambient, specular;
                }
            }
        }
    }
}