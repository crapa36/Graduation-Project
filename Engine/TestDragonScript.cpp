#include "pch.h"
#include "TestDragonScript.h"
#include "Input.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "Timer.h"
#include "Animator.h"

void TestDragonScript::Update() {
    if (INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
        int32 count = GetAnimator()->GetAnimCount();
        int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

        int32 index = (currentIndex + 1) % count;
        GetAnimator()->Play(index);
    }

    if (INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
        int32 count = GetAnimator()->GetAnimCount();
        int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

        int32 index = (currentIndex - 1 + count) % count;
        GetAnimator()->Play(index);
    }
}

void TestDragonScript::LateUpdate()
{
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {
        _centerPos.x = GEngine->GetWindow().width / 2;
        _centerPos.y = GEngine->GetWindow().height / 2;
        _centerScreenPos = _centerPos;
        ClientToScreen(GEngine->GetWindow().hwnd, &_centerScreenPos);
        if (INPUT->GetButton(KEY_TYPE::ESC))
            PostQuitMessage(0);

        if (INPUT->GetButtonDown(KEY_TYPE::DEL)) {
            GEngine->SetDebugMode(!GEngine->GetDebugMode());
        }

        if (INPUT->GetButtonDown(KEY_TYPE::END)) {
            GEngine->SetImguiMode(!GEngine->GetImguiMode());
        }   

        //int32 count = GetAnimator()->GetAnimCount();
        //int32 index = GetAnimator()->GetCurrentClipIndex();

        //if (INPUT->GetButton(KEY_TYPE::W)) {
        //    if (index != 2) {
        //        GetAnimator()->Play(2);
        //    }
        //}
        //if (INPUT->GetButton(KEY_TYPE::S)) {
        //    if (index != 2) {
        //        GetAnimator()->Play(2);
        //    }
        //}
        //if (INPUT->GetButton(KEY_TYPE::A)) {
        //    if (index != 2) {
        //        GetAnimator()->Play(2);
        //    }
        //}
        //if (INPUT->GetButton(KEY_TYPE::D)) {
        //    if (index != 2) {
        //        GetAnimator()->Play(2);
        //    }
        //}

        //if (!INPUT->GetButton(KEY_TYPE::W) && !INPUT->GetButton(KEY_TYPE::S) &&
        //    !INPUT->GetButton(KEY_TYPE::A) && !INPUT->GetButton(KEY_TYPE::D)) {
        //    if (index != 1)
        //        GetAnimator()->Play(1);
        //}

    }
}
