#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "PhysicsManager.h"
#include "Rigidbody.h"
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
    // 게임 윈도우가 포커스 되었을 때만 입력 처리
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {
        Vec3 revolution = GetTransform()->GetLocalRevolution();
        Vec3 pos = GetTransform()->GetLocalPosition();

        // 마우스 좌클릭 시 선택 또는 피킹 처리
        if (INPUT->IsMouseButtonPressed(0)) {  // 0은 왼쪽 마우스 버튼
            const POINT& mousePos = INPUT->GetMousePos();
            GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
        }


        // Alt 키 입력으로 마우스 잠금 전환
        if (INPUT->IsKeyJustPressed(DIK_LALT)) {
            _isMouseLock = !_isMouseLock;
        }

        if (_isMouseLock) {
            ShowCursor(false); // 마우스 커서를 숨김
            const POINT& currentMousePos = INPUT->GetMousePos();

            // 마우스 움직임에 따른 회전량 계산
            float deltaX = static_cast<float>(currentMousePos.x - _centerPos.x);
            float deltaY = static_cast<float>(currentMousePos.y - _centerPos.y);

            // 회전 민감도 조절
            float sensitivity = 0.005f;

            revolution.y += deltaX * sensitivity;
            revolution.x += deltaY * sensitivity;

            // 회전 각도 제한
            revolution.x = std::clamp(revolution.x, -XM_PIDIV2, XM_PIDIV2); // Pitch 제한
            revolution.y = std::fmod(revolution.y, XM_2PI); // Yaw 값을 -2PI ~ 2PI 범위로 제한

            // 회전 적용
            GetTransform()->SetLocalRevolution(revolution);

            // 마우스 커서를 화면 중앙으로 고정
            SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
        }
        else {
            ShowCursor(true); // 마우스 커서를 보임
        }

        GetTransform()->SetLocalRevolution(revolution);

        // 부모 객체(카메라)의 회전과 이동 처리
        shared_ptr<GameObject> parent = GetGameObject()->GetParent().lock();
        shared_ptr<Transform> parentTransform = parent->GetTransform();
        Vec3 parentRotate = parentTransform->GetLocalRotation();
        Vec3 parentPos = parentTransform->GetLocalPosition();

        Vec3 dir;

        if (INPUT->IsKeyPressed(DIK_W) || INPUT->IsKeyPressed(DIK_S) ||
            INPUT->IsKeyPressed(DIK_A) || INPUT->IsKeyPressed(DIK_D)) {

            dir = { 0.f, 0.f, 0.f };
            
            //방향벡터 구하기
            if (INPUT->IsKeyPressed(DIK_W)) {
                dir -= GetTransform()->GetLook();
            }
            if (INPUT->IsKeyPressed(DIK_S)) {
                dir += GetTransform()->GetLook();
            }
            if (INPUT->IsKeyPressed(DIK_A)) {
                dir += GetTransform()->GetRight();
            }
            if (INPUT->IsKeyPressed(DIK_D)) {
                dir -= GetTransform()->GetRight();
            }
            
            Vec3 look = GetTransform()->GetLook(); //카메라가 바라보는 벡터
            look.Normalize();
            dir.Normalize();

            
            //외적과 내적으로 두 벡터 사이의 각 구하기
            float dotProduct = look.Dot(dir);
            dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);
            float dirAngle = acos(dotProduct);
            if (isnan(dirAngle)) {
                dirAngle = XM_PI;
            }
            Vec3 crossProduct = look.Cross(dir);
            crossProduct.Normalize();
            double dirAnglePM = crossProduct.Dot({ 0.f, 1.f, 0.f });
            if (dirAnglePM < 0) {
                dirAngle = -dirAngle;
            }
            
            _dir.y = revolution.y + dirAngle; // 최종회전값

            if (parentRotate.y - _dir.y > XM_PI) {
                parentRotate.y -= 2 * XM_PI;
            }
            if (parentRotate.y - _dir.y < -XM_PI) {
                parentRotate.y += 2 * XM_PI;
            }
        }
        //회전보간
        Vec3 result;
        result.x = 0.f;
        result.y = _dir.y + (0.9) * (parentRotate.y - _dir.y);
        result.z = 0.f;

        parentTransform->SetLocalRotation(result);


        //WASD 입력시 캐릭터가 바라보는 방향으로 이동
        if (INPUT->IsKeyPressed(DIK_W) || INPUT->IsKeyPressed(DIK_S) ||
            INPUT->IsKeyPressed(DIK_A) || INPUT->IsKeyPressed(DIK_D)) { 
            parentPos -= parentTransform->GetLook() * _speed * DELTA_TIME;
        }

        // 스페이스바로 위쪽 이동, Ctrl로 아래쪽 이동
        if (INPUT->IsKeyPressed(DIK_SPACE)) {
            parentPos += parentTransform->GetUp() * _speed * DELTA_TIME;
        }
        if (INPUT->IsKeyPressed(DIK_LCONTROL)) {
            parentPos -= parentTransform->GetUp() * _speed * DELTA_TIME;
        }

        // Shift 키로 이동 속도 변경
        if (INPUT->IsKeyPressed(DIK_LSHIFT)) {
            _speed = 200.f;
        }
        else {
            _speed = 100.f;
        }


        // 마우스 휠로 줌 기능 추가
        int mouseWheel = INPUT->GetMouseWheel();
        Vec3 vectorAB = XMVectorSubtract(pos, Vec3(0.f, 0.f, 0.f));
        vectorAB.Normalize();
        if (mouseWheel != 0) {

            float zoomSpeed = 0.1f;
            pos -= vectorAB * zoomSpeed * mouseWheel * DELTA_TIME;
        }

        // 부모 객체 위치 적용
        GetTransform()->SetLocalPosition(pos);
        parentTransform->SetLocalPosition(parentPos);
    }
}
