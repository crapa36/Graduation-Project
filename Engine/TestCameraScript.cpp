#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "PhysicsManager.h"
#include "Engine.h"

TestCameraScript::TestCameraScript() {
    _centerPos.x = GEngine->GetWindow().width / 2;
    _centerPos.y = GEngine->GetWindow().height / 2;
    _centerScreenPos = _centerPos;
    ClientToScreen(GEngine->GetWindow().hwnd, &_centerScreenPos);
}

TestCameraScript::~TestCameraScript() {
}

void TestCameraScript::LateUpdate() {
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {
        Vec3 revolution = GetTransform()->GetLocalRevolution();

        if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON)) {
            const POINT& mousePos = INPUT->GetMousePos();
            GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
        }
        // 마우스 중앙 고정 및 화면 회전
        if (INPUT->GetButtonDown(KEY_TYPE::ALT)) {
            _isMouseLock = !_isMouseLock;
        }

        if (_isMouseLock) {
            ShowCursor(false);
            const POINT& currentMousePos = INPUT->GetMousePos();

            // 마우스 움직임에 따른 회전량 계산
            float deltaX = static_cast<float>(currentMousePos.x - _centerPos.x);
            float deltaY = static_cast<float>(currentMousePos.y - _centerPos.y);

            // 회전 민감도를 조절할 수 있는 변수
            float sensitivity = 0.005f;

            revolution.y += deltaX * sensitivity;
            revolution.x += deltaY * sensitivity;

            revolution.x = std::clamp(revolution.x, -XM_PIDIV2, XM_PIDIV2); // Pitch 제한
            revolution.y = std::fmod(revolution.y, XM_2PI); // Yaw 값을 -2PI ~ 2PI 범위로 제한
            GetTransform()->SetLocalRevolution(revolution);
            SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
        }
        else {
            ShowCursor(true);
        }


        shared_ptr<Transform> parent = GetTransform()->GetParent().lock();
        Vec3 parentRotate = parent->GetLocalRotation();
        Vec3 parentPos = parent->GetLocalPosition();

        if (INPUT->GetButton(KEY_TYPE::W) || INPUT->GetButton(KEY_TYPE::S) ||
            INPUT->GetButton(KEY_TYPE::A) || INPUT->GetButton(KEY_TYPE::D)) {
            
            Vec3 _dir = { 0.f, 0.f, 0.f };

            if (INPUT->GetButton(KEY_TYPE::W)) {
                _dir -= GetTransform()->GetLook();
            }
            if (INPUT->GetButton(KEY_TYPE::S)) {
                _dir += GetTransform()->GetLook();
            }
            if (INPUT->GetButton(KEY_TYPE::A)) {
                _dir += GetTransform()->GetRight();
            }
            if (INPUT->GetButton(KEY_TYPE::D)) {
                _dir -= GetTransform()->GetRight();
            }
            Vec3 dir = GetTransform()->GetLook();
            dir.Normalize();
            _dir.Normalize();
            float dirAngle = acos(dir.Dot(_dir));
            Vec3 crossProduct = dir.Cross(_dir);
            float dirAnglePM = crossProduct.Dot({ 0.f, 1.f, 0.f });

            if (dirAnglePM < 0) {
                dirAngle = -dirAngle;
            }
          
            parentRotate.y = revolution.y + dirAngle;
        }
        if (INPUT->GetButton(KEY_TYPE::SPACE))
            parentPos += parent->GetUp() * _speed * DELTA_TIME;

        if (INPUT->GetButton(KEY_TYPE::CTRL))
            parentPos -= parent->GetUp() * _speed * DELTA_TIME;

        if (INPUT->GetButtonDown(KEY_TYPE::SHIFT))
            _speed = 200.f;

        if (INPUT->GetButtonUp(KEY_TYPE::SHIFT))
            _speed = 100.f;

        parent->SetLocalRotation(parentRotate);

        if (INPUT->GetButton(KEY_TYPE::W) || INPUT->GetButton(KEY_TYPE::S) ||
            INPUT->GetButton(KEY_TYPE::A) || INPUT->GetButton(KEY_TYPE::D)) {
            parentPos -= parent->GetLook() * _speed * DELTA_TIME;
        }
        parent->SetLocalPosition(parentPos);
    }
}