#include "pch.h"
#include "TestDragonScript.h"
#include "Input.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "Timer.h"
#include "Animator.h"

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

        // ESC 키로 종료
        if (INPUT->IsKeyPressed(DIK_ESCAPE))
            PostQuitMessage(0);

        // DEL 키로 디버그 모드 전환
        if (INPUT->IsKeyJustPressed(DIK_DELETE)) {
            GEngine->SetDebugMode(!GEngine->GetDebugMode());
        }

        // END 키로 ImGui 모드 전환
        if (INPUT->IsKeyJustPressed(DIK_END)) {
            GEngine->SetImguiMode(!GEngine->GetImguiMode());
        }

        // (주석 처리된) WASD 키 입력에 따라 애니메이션 재생 코드
        // 각 키 입력에 따라 다른 애니메이션 클립 재생 가능
        /*
        int32 index = GetAnimator()->GetCurrentClipIndex();
        if (INPUT->IsKeyPressed(DIK_W)) {
            if (index != 2) {
                GetAnimator()->Play(2);
            }
        }
        if (INPUT->IsKeyPressed(DIK_S)) {
            if (index != 2) {
                GetAnimator()->Play(2);
            }
        }
        if (INPUT->IsKeyPressed(DIK_A)) {
            if (index != 2) {
                GetAnimator()->Play(2);
            }
        }
        if (INPUT->IsKeyPressed(DIK_D)) {
            if (index != 2) {
                GetAnimator()->Play(2);
            }
        }

        // W, A, S, D 키가 눌리지 않을 때 정지 애니메이션으로 전환
        if (!INPUT->IsKeyPressed(DIK_W) && !INPUT->IsKeyPressed(DIK_S) &&
            !INPUT->IsKeyPressed(DIK_A) && !INPUT->IsKeyPressed(DIK_D)) {
            if (index != 1)
                GetAnimator()->Play(1);
        }
        */
    }
}
