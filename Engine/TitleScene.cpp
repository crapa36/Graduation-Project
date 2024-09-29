#include "pch.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Shader.h"
#include "Material.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "TitleScript.h"

#include "Resources.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Rigidbody.h"
#include "MeshRenderer.h"

TitleScene::TitleScene() {
#pragma region UI_Camera
    {
        shared_ptr<GameObject> camera = make_shared<GameObject>();
        camera->SetName(L"Orthographic_Camera");
        camera->AddComponent(make_shared<Transform>());
        camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
        camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
        camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
        uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
        camera->GetCamera()->SetCullingMaskAll();
        camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI
        _scene->AddGameObject(camera);
    }
#pragma endregion

#pragma region Title

    shared_ptr<GameObject> obj = make_shared<GameObject>();
    obj->SetName(L"Title");
    obj->SetLayerIndex(GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI"));
    obj->AddComponent(make_shared<Transform>());

    obj->GetTransform()->SetLocalScale(Vec3(GEngine->GetWindow().clientWidth, GEngine->GetWindow().clientHeight, 1.f));
    obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));
    shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
    {
        shared_ptr<Mesh> mesh = GET_SINGLETON(Resources)->LoadRectangleMesh();
        meshRenderer->SetMesh(mesh);
    }
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Texture");

        shared_ptr<Texture> texture;

        texture = GET_SINGLETON(Resources)->Load<Texture>(L"Title", L"..\\Resources\\Texture\\Title.png");

        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        meshRenderer->SetMaterial(material);
    }
    obj->AddComponent(make_shared<TitleScript>());
    obj->AddComponent(meshRenderer);
    _scene->AddGameObject(obj);

#pragma endregion
}

TitleScene::~TitleScene() {
}