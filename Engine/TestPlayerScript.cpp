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

        // ����Ű �Է¿� ���� ȸ�� ó��
        if (INPUT->IsKeyPressed(DIK_UP))  // ���� ����Ű
            rotation.x += DELTA_TIME * 1.f;
        if (INPUT->IsKeyPressed(DIK_DOWN))  // �Ʒ��� ����Ű
            rotation.x -= DELTA_TIME * 1.f;
        if (INPUT->IsKeyPressed(DIK_LEFT))  // ���� ����Ű
            rotation.y += DELTA_TIME * 1.f;
        if (INPUT->IsKeyPressed(DIK_RIGHT))  // ������ ����Ű
            rotation.y -= DELTA_TIME * 1.f;

        // ��ȯ�� ȸ������ ����
        GetTransform()->SetLocalRotation(rotation);
    }
}
