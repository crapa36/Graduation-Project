#include "pch.h"
#include "TestPlayerScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "PhysicsManager.h"
#include "Engine.h"
#include "Rigidbody.h"

TestPlayerScript::TestPlayerScript() {
}

TestPlayerScript::~TestPlayerScript() {
}

void TestPlayerScript::LateUpdate() {
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {
        Vec3 rotation = GetTransform()->GetLocalRotation();

        // 방향키 입력에 따른 회전 처리
        if (INPUT->IsKeyPressed(DIK_UP))  // 위쪽 방향키
            rotation.x += DELTA_TIME * 1.f;
        if (INPUT->IsKeyPressed(DIK_DOWN))  // 아래쪽 방향키
            rotation.x -= DELTA_TIME * 1.f;
        if (INPUT->IsKeyPressed(DIK_LEFT))  // 왼쪽 방향키
            rotation.y += DELTA_TIME * 1.f;
        if (INPUT->IsKeyPressed(DIK_RIGHT))  // 오른쪽 방향키
            rotation.y -= DELTA_TIME * 1.f;

        // 변환된 회전값을 적용
        GetTransform()->SetLocalRotation(rotation);
    }
}
