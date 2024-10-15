#include "pch.h"
#include "TestDragonScript.h"
#include "Input.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "Timer.h"
#include "Animator.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Scene.h"

void TestDragonScript::Update() {

    // 1번 키 입력 시 애니메이션 순차 재생
    if (INPUT->IsKeyJustPressed(DIK_1)) {
        int32 count = GetAnimator()->GetAnimCount();
        int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

        int32 index = (currentIndex + 1) % count;  // 다음 애니메이션
        GetAnimator()->Play(index);
    }

    // 2번 키 입력 시 애니메이션 역순 재생
    if (INPUT->IsKeyJustPressed(DIK_2)) {
        int32 count = GetAnimator()->GetAnimCount();
        int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

        int32 index = (currentIndex - 1 + count) % count;  // 이전 애니메이션
        GetAnimator()->Play(index);
    }
}

void TestDragonScript::LateUpdate() {
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {

        // 화면 중심 좌표 계산

        // ESC 키로 메뉴 토글
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

        // DEL 키로 디버그 모드 전환
        if (INPUT->IsKeyJustPressed(DIK_DELETE)) {
            GEngine->SetDebugMode(!GEngine->GetDebugMode());
        }

        // END 키로 ImGui 모드 전환
        if (INPUT->IsKeyJustPressed(DIK_END)) {
            GEngine->SetImguiMode(!GEngine->GetImguiMode());
        }
    }
}