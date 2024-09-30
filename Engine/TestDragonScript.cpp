#include "pch.h"
#include "TestDragonScript.h"
#include "Input.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "Timer.h"
#include "Animator.h"

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

        // ESC Ű�� ����
        if (INPUT->IsKeyPressed(DIK_ESCAPE))
            PostQuitMessage(0);

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
