#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "ParticleSystem.h"

#include "TestCameraScript.h"
#include "TestDragonScript.h"

#include "Resources.h"
#include "Terrain.h"
#include "SphereCollider.h"
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
void SceneManager::LoadScene(wstring sceneName) {

    // TODO : 기존 Scene 정리
    // TODO : 파일에서 Scene 정보 로드

    _activeScene = LoadTestScene();

    _activeScene->Awake();
    _activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name) {

    // 기존 데이터 삭제
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
#pragma region LayerMask
    SetLayerName(0, L"Default");
    SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"ComputeShader");

        // UAV 용 Texture 생성
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->CreateTexture(L"UAVTexture",
                                                                              DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
                                                                              CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
                                                                              D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"ComputeShader");
        material->SetShader(shader);
        material->SetInt(0, 1);
        GEngine->GetComputeDescriptorHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

        // 쓰레드 그룹 (1 * 1024 * 1)
        material->Dispatch(1, 1024, 1);
    }
#pragma endregion

    shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
    {
        shared_ptr<GameObject> camera = make_shared<GameObject>();
        camera->SetName(L"Main_Camera");
        camera->AddComponent(make_shared<Transform>());
        camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
        camera->GetCamera()->SetFar(10000.f); // Far 10000 으로
        camera->AddComponent(make_shared<TestCameraScript>());
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

        uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
        scene->AddGameObject(camera);
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
        camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
        scene->AddGameObject(camera);
    }
#pragma endregion

#pragma region SkyBox
    {
        shared_ptr<GameObject> skybox = make_shared<GameObject>();
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
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<SphereCollider>());
        obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
        obj->GetTransform()->SetLocalPosition(Vec3(200, 50.f, 50.f));

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
        dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetRadius(0.5f);

        obj->AddComponent(meshRenderer);
        scene->AddGameObject(obj);
    }
#pragma endregion

#pragma region Terrain
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<Terrain>());
        obj->AddComponent(make_shared<MeshRenderer>());

        obj->GetTransform()->SetLocalScale(Vec3(50.f, 400.f, 50.f));
        obj->GetTransform()->SetLocalPosition(Vec3(-1600.f, -400.f, -1600.f));
        obj->SetStatic(true);
        obj->GetTerrain()->Init(64, 64);
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
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> frustumMesh = GET_SINGLETON(Resources)->LoadCameraFrustumMesh(light->GetLight()->GetShadowCamera()->GetCamera());
            meshRenderer->SetMesh(frustumMesh);
        }
        {
            shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Frustum");
            meshRenderer->SetMaterial(material->Clone());
        }
        light->AddComponent(meshRenderer);
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
        light->GetTransform()->SetLocalPosition(Vec3(300.f, 100.f, 300.f));
        light->AddComponent(make_shared<Light>());
        light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));
        light->GetLight()->SetLightType(LIGHT_TYPE::SPOT);
        light->GetLight()->SetDiffuse(Vec3(0.0f, 0.f, 1.f));
        light->GetLight()->SetAmbient(Vec3(0.0f, 0.0f, 0.1f));
        light->GetLight()->SetSpecular(Vec3(0.0f, 0.0f, 0.1f));
        light->GetLight()->SetLightRange(200.f);
        light->GetLight()->SetLightAngle(3.14f / 2);

        scene->AddGameObject(light);
    }
#pragma endregion

    //#pragma region ParticleSystem
    //    {
    //        shared_ptr<GameObject> particle = make_shared<GameObject>();
    //        particle->AddComponent(make_shared<Transform>());
    //        particle->AddComponent(make_shared<ParticleSystem>());
    //        particle->SetCheckFrustum(false);
    //        particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
    //        scene->AddGameObject(particle);
    //    }
    //#pragma endregion

#pragma region FBX
    {
        shared_ptr<MeshData> meshData = GET_SINGLETON(Resources)->LoadFBX(L"..\\Resources\\FBX\\Dragon.fbx");

        vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

        for (auto& gameObject : gameObjects) {
            gameObject->SetName(L"Dragon");
            gameObject->SetCheckFrustum(false);
            gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, -100.f, 100.f));
            gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
            scene->AddGameObject(gameObject);
            gameObject->SetStatic(false);
            gameObject->AddComponent(make_shared<TestDragonScript>());
        }
    }
#pragma endregion

    return scene;
}