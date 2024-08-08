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
        _centerPos.x = GEngine->GetWindow().width / 2;
        _centerPos.y = GEngine->GetWindow().height / 2;
        _centerScreenPos = _centerPos;
        ClientToScreen(GEngine->GetWindow().hwnd, &_centerScreenPos);
        if (INPUT->GetButton(KEY_TYPE::ESC))
            PostQuitMessage(0);

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

        if (INPUT->GetButtonDown(KEY_TYPE::SHIFT))
            _speed = 200.f;

        if (INPUT->GetButtonUp(KEY_TYPE::SHIFT))
            _speed = 100.f;

        if (INPUT->GetButtonDown(KEY_TYPE::DEL)) {
            GEngine->SetImguiMode(!GEngine->GetImguiMode());
        }

        if (INPUT->GetButtonDown(KEY_TYPE::END)) {
            GEngine->SetImguiMode(!GEngine->GetImguiMode());
        }

        // 마우스 중앙 고정 및 화면 회전
        if (INPUT->GetButtonDown(KEY_TYPE::ALT)) {
            _isMouseLock = !_isMouseLock;
        }

        if (_isMouseLock) {
            ShowCursor(false);
            const POINT& currentMousePos = INPUT->GetMousePos();
            Vec3 rotation = GetTransform()->GetLocalRotation();

            // 마우스 움직임에 따른 회전량 계산
            float deltaX = static_cast<float>(currentMousePos.x - _centerPos.x);
            float deltaY = static_cast<float>(currentMousePos.y - _centerPos.y);

            // 회전 민감도를 조절할 수 있는 변수
            float sensitivity = 0.005f;

            rotation.y += deltaX * sensitivity;
            rotation.x += deltaY * sensitivity;

            rotation.x = std::clamp(rotation.x, -XM_PIDIV2, XM_PIDIV2); // Pitch 제한
            rotation.y = std::fmod(rotation.y, XM_2PI); // Yaw 값을 -2PI ~ 2PI 범위로 제한
            GetTransform()->SetLocalRotation(rotation);
            SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
        }
        else {
            ShowCursor(true);
        }
        if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON)) {
            const POINT& mousePos = INPUT->GetMousePos();
            GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
        }

        GetTransform()->SetLocalPosition(pos);
    }
}