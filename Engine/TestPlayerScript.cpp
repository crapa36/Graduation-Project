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
        if (INPUT->GetButton(KEY_TYPE::UP))
            rotation.x += DELTA_TIME * 1.f;
        if (INPUT->GetButton(KEY_TYPE::DOWN))
            rotation.x -= DELTA_TIME * 1.f;
        if (INPUT->GetButton(KEY_TYPE::LEFT))
            rotation.y += DELTA_TIME * 1.f;
        if (INPUT->GetButton(KEY_TYPE::RIGHT))
            rotation.y -= DELTA_TIME * 1.f;

        GetTransform()->SetLocalRotation(rotation);
    }
}