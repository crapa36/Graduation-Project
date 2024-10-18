#include "pch.h"
#include "TestDragonScript.h"
#include "Input.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "Transform.h"
#include "Timer.h"
#include "Rigidbody.h"
#include "Animator.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "MeshRenderer.h"
#include "Resources.h"



void TestDragonScript::Update() {

    // 1�� Ű �Է� �� �ִϸ��̼� ���� ���
    if (INPUT->IsKeyJustPressed(DIK_1)) {
        int32 count = GetAnimator()->GetAnimCount();
        int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

        int32 index = (currentIndex + 1) % count;  // ���� �ִϸ��̼�
        GetAnimator()->Play(index);
    }

    // 2�� Ű �Է� �� �ִϸ��̼� ���� ���
    if (INPUT->IsKeyJustPressed(DIK_2)) {
        int32 count = GetAnimator()->GetAnimCount();
        int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

        int32 index = (currentIndex - 1 + count) % count;  // ���� �ִϸ��̼�
        GetAnimator()->Play(index);
    }   
}

void TestDragonScript::LateUpdate() {
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {

        // ȭ�� �߽� ��ǥ ���

        // ESC Ű�� �޴� ���
        if (INPUT->IsKeyJustPressed(DIK_ESCAPE)) {
            //PostQuitMessage(0);
            auto gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
            for (auto& gameObject : gameObjects) {
                if (gameObject->GetName() == L"Menu")
                    gameObject->SetEnable(!gameObject->IsEnable());
                else if (gameObject->GetName() == L"MenuExitButton")
                    gameObject->SetEnable(!gameObject->IsEnable());
            }
        }

        

        // DEL Ű�� ����� ��� ��ȯ
        if (INPUT->IsKeyJustPressed(DIK_DELETE)) {
            GEngine->SetDebugMode(!GEngine->GetDebugMode());
        }

        // END Ű�� ImGui ��� ��ȯ
        if (INPUT->IsKeyJustPressed(DIK_END)) {
            GEngine->SetImguiMode(!GEngine->GetImguiMode());
        }
    }
}

void TestDragonScript::MakeBullet() {
    shared_ptr<GameObject> bullet = make_shared<GameObject>();
    
    bullet->AddComponent(make_shared<Transform>());
    bullet->AddComponent(make_shared<SphereCollider>());
    bullet->AddComponent(make_shared<Rigidbody>());
    shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
    {
        shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadSphereMesh();
        meshRenderer->SetMesh(sphereMesh);
    }
    {
        shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Pebbles");
        meshRenderer->SetMaterial(material->Clone());
    }
    bullet->AddComponent(meshRenderer);

    
    bullet->SetName(L"Bullet");
    bullet->GetTransform()->SetParent(GetTransform());
    bullet->GetTransform()->SetInheritRevolution(false);
    bullet->GetTransform()->SetInheritRotation(false);

    bullet->GetCollider()->SetRadius(10.f);
    bullet->GetRigidbody()->SetVelocity(GetTransform()->GetLook());
    bullet->SetEnable(true);
    GET_SINGLETON(SceneManager)->GetActiveScene()->AddGameObject(bullet);
}