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
#include "BulletScript.h"

void TestDragonScript::Update() {

    //// 1�� Ű �Է� �� �ִϸ��̼� ���� ���
    //if (INPUT->IsKeyJustPressed(DIK_1)) {
    //    int32 count = GetAnimator()->GetAnimCount();
    //    int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

    //    int32 index = (currentIndex + 1) % count;  // ���� �ִϸ��̼�
    //    GetAnimator()->Play(index);
    //}

    //// 2�� Ű �Է� �� �ִϸ��̼� ���� ���
    //if (INPUT->IsKeyJustPressed(DIK_2)) {
    //    int32 count = GetAnimator()->GetAnimCount();
    //    int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

    //    int32 index = (currentIndex - 1 + count) % count;  // ���� �ִϸ��̼�
    //    GetAnimator()->Play(index);
    //}



    if (INPUT->IsKeyJustPressed(DIK_F)) {
        bool reuse = false;
        for (auto& gameObject : _bullets) {
            if (!gameObject->IsEnable()) {
                gameObject->SetEnable(true);
                auto objLookVec = GetTransform()->GetLook();
                objLookVec.Normalize();
                auto bulletStartPos = GetTransform()->GetWorldPosition() + GetCollider()->GetCenter() + objLookVec * (GetCollider()->GetRadius() + gameObject->GetCollider()->GetRadius() + 0.001f);
                gameObject->GetTransform()->SetLocalPosition(bulletStartPos);
                gameObject->GetRigidbody()->SetVelocity(objLookVec * 200.f);
                reuse = true;
                break;
            }
        }
        if (!reuse) {
            shared_ptr<GameObject> bullet = make_shared<GameObject>();

            bullet->SetName(L"Bullet");

            bullet->AddComponent(make_shared<Transform>());
            bullet->AddComponent(make_shared<BulletScript>());

            bullet->AddComponent(make_shared<SphereCollider>());
            bullet->AddComponent(make_shared<Rigidbody>());

            shared_ptr<MeshRenderer> bulletRenderer = make_shared<MeshRenderer>();
            {
                shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadSphereMesh();
                bulletRenderer->SetMesh(sphereMesh);
            }
            {
                shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Pebbles");
                bulletRenderer->SetMaterial(material);
            }
            bullet->AddComponent(bulletRenderer);

            dynamic_pointer_cast<SphereCollider>(bullet->GetCollider())->SetRadius(10.f);
            dynamic_pointer_cast<SphereCollider>(bullet->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));
            //ĳ������ �浹�ڽ��� ��ġ�� �ʰ� ��ġ�� ����.
            auto objLookVec = GetTransform()->GetLook();
            objLookVec.Normalize();
            auto bulletStartPos = GetTransform()->GetWorldPosition() + GetCollider()->GetCenter() + objLookVec * (GetCollider()->GetRadius() + bullet->GetCollider()->GetRadius() + 0.001f);

            bullet->GetTransform()->SetLocalPosition(bulletStartPos);
            bullet->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
            bullet->GetTransform()->SetInheritScale(false);
            bullet->GetTransform()->SetInheritPosition(false);
            bullet->GetRigidbody()->SetVelocity(objLookVec * 200.f);
            bullet->GetRigidbody()->SetUseGravity(false);
            bullet->GetRigidbody()->SetElasticity(0.0f);
            bullet->GetRigidbody()->SetDrag(0.1f);

            _bullets.push_back(bullet); 
            GET_SINGLETON(SceneManager)->GetActiveScene()->AddGameObject(bullet);
        }
    }
}

void TestDragonScript::LateUpdate() {
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {
        auto gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();

        // ESC Ű�� �޴� ���
        if (INPUT->IsKeyJustPressed(DIK_ESCAPE)) {

            //PostQuitMessage(0);
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