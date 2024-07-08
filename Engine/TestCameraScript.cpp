#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "PhysicsManager.h"

TestCameraScript::TestCameraScript() {
}

TestCameraScript::~TestCameraScript() {
}

void TestCameraScript::LateUpdate() {
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (INPUT->GetButton(KEY_TYPE::W))
        pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::S))
        pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::A))
        pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::D))
        pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::SPACE))
        pos += GetTransform()->GetUp() * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::CTRL))
        pos -= GetTransform()->GetUp() * _speed * DELTA_TIME;

    if (INPUT->GetButton(KEY_TYPE::Q)) {
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.x += DELTA_TIME * 1.f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButton(KEY_TYPE::E)) {
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.x -= DELTA_TIME * 1.f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButton(KEY_TYPE::UP)) {
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.x -= DELTA_TIME * 1.f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButton(KEY_TYPE::DOWN)) {
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.x += DELTA_TIME * 1.f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButton(KEY_TYPE::LEFT)) {
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.y -= DELTA_TIME * 1.f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButton(KEY_TYPE::RIGHT)) {
        Vec3 rotation = GetTransform()->GetLocalRotation();
        rotation.y += DELTA_TIME * 1.f;
        GetTransform()->SetLocalRotation(rotation);
    }

    if (INPUT->GetButton(KEY_TYPE::SHIFT)) {
        _speed = 200.f;
    }
    else {
        _speed = 100.f;
    }

    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON)) {
        _prevMousePos = INPUT->GetMousePos(); // ���콺 Ŭ�� �� ���� ���콺 ��ġ ����
    }

    if (INPUT->GetButton(KEY_TYPE::LBUTTON)) {
        const POINT& currentMousePos = INPUT->GetMousePos();
        Vec3 rotation = GetTransform()->GetLocalRotation();

        // ���콺 �����ӿ� ���� ȸ���� ���
        float deltaX = static_cast<float>(currentMousePos.x - _prevMousePos.x);
        float deltaY = static_cast<float>(currentMousePos.y - _prevMousePos.y);

        // ȸ�� �ΰ����� ������ �� �ִ� ����
        float sensitivity = 0.005f;

        rotation.y += deltaX * sensitivity;
        rotation.x += deltaY * sensitivity;

        GetTransform()->SetLocalRotation(rotation);

        // ���� ���콺 ��ġ�� ���� ��ġ�� ������Ʈ
        _prevMousePos = currentMousePos;
    }

    if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON)) {
        const POINT& mousePos = INPUT->GetMousePos();
        GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
    }

    GetTransform()->SetLocalPosition(pos);
}