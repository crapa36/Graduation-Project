#include "pch.h"
#include "TestScene.h"
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
#include "TestReflection.h"
#include "TestWaterScript.h"

#include "Resources.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Rigidbody.h"

#include "MeshData.h"

TestScene::TestScene() {
#pragma region FBX
    {
        shared_ptr<MeshData> meshData = GET_SINGLETON(Resources)->LoadFBX(L"..\\Resources\\FBX\\Dragon.fbx");

        vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

        for (auto& gameObject : gameObjects) {
            gameObject->SetName(L"Dragon");
            gameObject->SetCheckFrustum(false);

            gameObject->AddComponent(make_shared<BoxCollider>());
            gameObject->AddComponent(make_shared<Rigidbody>());
            dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider())->SetExtents(Vec3(40.f, 30.f, 40.f));
            dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider())->SetCenter(Vec3(0.f, 10.f, 0.f));
            gameObject->GetTransform()->SetLocalPosition(Vec3(210.f, 500.f, 0.f));
            gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
            gameObject->GetRigidbody()->SetUseGravity(true);
            gameObject->GetRigidbody()->SetElasticity(0.0f);

            gameObject->SetStatic(false);
            _scene->AddGameObject(gameObject);
        }
    }
#pragma endregion

#pragma region BIN
    {
        wstring path = L"../Resources/BIN/Gunship.bin";
        shared_ptr<MeshData> meshData = GET_SINGLETON(Resources)->LoadBIN(path);

        shared_ptr<GameObject> mainObject = make_shared<GameObject>();

mainObject->SetName(L"Main");
mainObject->SetCheckFrustum(false);

mainObject->AddComponent(make_shared<Transform>());
shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
{
    shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadPointMesh();
    meshRenderer->SetMesh(sphereMesh);
}
{
    shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Pebbles");
    meshRenderer->SetMaterial(material->Clone());
}
mainObject->AddComponent(meshRenderer);

        mainObject->AddComponent(make_shared<BoxCollider>());
        mainObject->AddComponent(make_shared<TestDragonScript>());
        mainObject->AddComponent(make_shared<Rigidbody>());
        dynamic_pointer_cast<BoxCollider>(mainObject->GetCollider())->SetExtents(Vec3(40.f, 30.f, 40.f));
        dynamic_pointer_cast<BoxCollider>(mainObject->GetCollider())->SetCenter(Vec3(0.f, 10.f, 0.f));
        mainObject->GetTransform()->SetLocalPosition(Vec3(0.f, 500.f, 0.f));
        mainObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
        mainObject->GetRigidbody()->SetUseGravity(false);
        mainObject->GetRigidbody()->SetElasticity(0.0f);
        mainObject->GetRigidbody()->SetDrag(0.95f);

mainObject->SetStatic(false);
_scene->AddGameObject(mainObject);

vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

for (auto& gameObject : gameObjects) {
    gameObject->SetCheckFrustum(false);

    gameObject->SetParent(mainObject);

    gameObject->SetStatic(false);
    _scene->AddGameObject(gameObject);  
}
#pragma region Camera
{
    shared_ptr<GameObject> camera = make_shared<GameObject>();
    camera->SetName(L"Main_Camera");
    camera->AddComponent(make_shared<Transform>());
    camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
    camera->AddComponent(make_shared<TestCameraScript>());
    camera->GetCamera()->SetFar(10000.f); // Far 10000 ����
    camera->GetTransform()->SetLocalPosition(Vec3(0.f, 50.f, -200.f));

    camera->SetParent(mainObject);
    camera->GetTransform()->SetInheritRotation(false);
    camera->GetTransform()->SetInheritScale(false);

    uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
    //camera->GetCamera()->SetCullingMaskAll(); // �� ����
    camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
    _scene->AddGameObject(camera);
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
        _scene->AddGameObject(camera);
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
            shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadCubeMesh();
            meshRenderer->SetMesh(sphereMesh);
        }
        {
            shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Skybox");
            shared_ptr<CubeMapTexture> texture = GET_SINGLETON(Resources)->Load<CubeMapTexture>(L"Sky01", L"..\\Resources\\Texture\\SkyBox_0.dds");

            shared_ptr<Material> material = make_shared<Material>();
            material->SetShader(shader);
            material->SetCubeMapTexture(texture);
            meshRenderer->SetMaterial(material);
        }
        skybox->AddComponent(meshRenderer);
        _scene->AddGameObject(skybox);
    }
#pragma endregion

#pragma region Object
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<SphereCollider>());
        obj->AddComponent(make_shared<Rigidbody>());
        obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
        obj->GetTransform()->SetLocalPosition(Vec3(200, 1000.f, 50.f));

        obj->SetCheckFrustum(false);
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
        _scene->AddGameObject(obj);
    }
#pragma endregion

#pragma region Terrain
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<Terrain>());
        obj->AddComponent(make_shared<BoxCollider>());
        obj->AddComponent(make_shared<MeshRenderer>());
        obj->SetCheckFrustum(false);

        obj->GetTransform()->SetLocalScale(Vec3(50.f, 400.f, 50.f));
        obj->GetTransform()->SetLocalPosition(Vec3(-1600.f, -200.f, -1600.f));
        obj->SetStatic(true);
        obj->GetTerrain()->Init(64, 64);
        dynamic_pointer_cast<BoxCollider>(obj->GetCollider())->SetCenter(Vec3(1600.f, 0.f, 1600.f));
        dynamic_pointer_cast<BoxCollider>(obj->GetCollider())->SetExtents(Vec3(3200.f, 1.f, 3200.f));

        obj->SetCheckFrustum(false);

        _scene->AddGameObject(obj);
    }
#pragma endregion

#pragma region UI_Test
    auto height = GEngine->GetWindow().clientHeight;
    auto width = GEngine->GetWindow().clientWidth;

    for (int32 i = 0; i < 6; i++) {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->SetLayerIndex(GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI")); // UI
        obj->AddComponent(make_shared<Transform>());

        // 비율로 설정

        obj->GetTransform()->SetLocalScale(Vec3(width / 8, width / 8, 200.f));
        obj->GetTransform()->SetLocalPosition(Vec3(-width / 2 + ((i + 0.5) * (width / 6)), height / 8 * 3, 500.f));

        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> mesh = GET_SINGLETON(Resources)->LoadRectangleMesh();
            meshRenderer->SetMesh(mesh);
        }
        {
            shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Texture");

            shared_ptr<Texture> texture;
            if (i < 3)
                texture = GET_SINGLETON(Resources)->Get<Texture>(L"ReflectionDiffuseTarget");
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
        _scene->AddGameObject(obj);
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
        _scene->AddGameObject(light);
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

        _scene->AddGameObject(light);
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
        light->GetLight()->SetLightAngle(3.14f / 2);

        _scene->AddGameObject(light);
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
        _scene->AddGameObject(particle);
    }
#pragma endregion

#pragma region Water
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->SetName(L"Water");
        obj->AddComponent(make_shared<Transform>());
        obj->AddComponent(make_shared<TestWaterScript>());
        obj->GetTransform()->SetLocalScale(Vec3(1000.f, 1.f, 1000.f));
        obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 50.f));
        obj->SetStatic(true);
        shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
        {
            shared_ptr<Mesh> waterMesh = GET_SINGLETON(Resources)->LoadPlaneMesh();
            meshRenderer->SetMesh(waterMesh);
        }
        {
            shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Water");
            meshRenderer->SetMaterial(material);
        }
        obj->AddComponent(meshRenderer);

    #pragma region Reflection_Camera
            {
                shared_ptr<GameObject> reflection_camera = make_shared<GameObject>();
                reflection_camera->SetName(L"Reflection_Camera");
                reflection_camera->AddComponent(make_shared<Transform>());
                reflection_camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
                reflection_camera->AddComponent(make_shared<TestReflection>());
                reflection_camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
                reflection_camera->GetCamera()->SetFar(10000.f); // Far 10000 ����
                reflection_camera->GetTransform()->SetParent(obj->GetTransform());
                reflection_camera->GetTransform()->SetInheritPosition(false);
                reflection_camera->GetTransform()->SetInheritRotation(false);
                reflection_camera->GetTransform()->SetInheritScale(false);

                uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");

                //reflection_camera->GetCamera()->SetCullingMaskAll(); // �� ����
                reflection_camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� ����
                _scene->AddGameObject(reflection_camera);
            }
    #pragma endregion

        _scene->AddGameObject(obj);
    }
#pragma endregion
}
TestScene::~TestScene() {
}