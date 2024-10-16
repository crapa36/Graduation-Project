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
#include "BaseCollider.h"

// 상수 정의
const float DEFAULT_SPEED = 50.f;
const float BOOSTED_SPEED = 100.f;
const float SENSITIVITY = 0.005f;
const float ZOOM_SPEED = 0.2f;
const float MIN_ZOOM_DISTANCE = 20.f;
const float MAX_ZOOM_DISTANCE = 300.f;

TestCameraScript::TestCameraScript() {
    _centerPos.x = GEngine->GetWindow().width / 2;
    _centerPos.y = GEngine->GetWindow().height / 2;
    _centerScreenPos = _centerPos;
    ClientToScreen(GEngine->GetWindow().hwnd, &_centerScreenPos);
}

TestCameraScript::~TestCameraScript() {
}

void TestCameraScript::LateUpdate() {
    if (GetForegroundWindow() != GEngine->GetWindow().hwnd) return;

    Vec3 revolution = GetTransform()->GetLocalRevolution();
    Vec3 pos = GetTransform()->GetLocalPosition();

    // 마우스 좌클릭 시 선택 또는 피킹 처리
    if (INPUT->IsMouseButtonPressed(0)) {
        const POINT& mousePos = INPUT->GetMousePos();
        GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
    }

    // Alt 키로 마우스 잠금 전환
    _isMouseLock = !INPUT->IsKeyPressed(DIK_LALT);

    if (_isMouseLock) {
        INPUT->SetCurrentCursor(L"null");
        if (ShowCursor(false) >= 0) ShowCursor(false);
        const POINT& currentMousePos = INPUT->GetMousePos();

        // 마우스 움직임에 따른 회전량 계산
        float deltaX = static_cast<float>(currentMousePos.x - _centerPos.x);
        float deltaY = static_cast<float>(currentMousePos.y - _centerPos.y);

        revolution.y += deltaX * SENSITIVITY;
        revolution.x += deltaY * SENSITIVITY;

        // 회전 각도 제한 (Pitch는 -80도 ~ 80도, Yaw는 -2PI ~ 2PI 범위로 제한)
        revolution.x = std::clamp(revolution.x, -XM_PIDIV2, XM_PIDIV2); // Pitch 제한
        revolution.y = std::fmod(revolution.y, XM_2PI); // Yaw 값을 -2PI ~ 2PI 범위로 제한
        SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
    }
    else {
        INPUT->SetCurrentCursor(L"Arrow");
        if (ShowCursor(true) < 0) ShowCursor(true);
    }

    GetTransform()->SetLocalRevolution(revolution);

    // Shift 키로 이동 속도 변경 (가속 효과 적용)
    float targetSpeed = INPUT->IsKeyPressed(DIK_LSHIFT) ? BOOSTED_SPEED : DEFAULT_SPEED;
    _speed = _speed + (targetSpeed - _speed) * DELTA_TIME * 5.0f; // 부드러운 가속
   

    shared_ptr<GameObject> parent = GetGameObject()->GetParent().lock();
    shared_ptr<Transform> parentTransform = parent->GetTransform();
    Vec3 parentRotate = parentTransform->GetLocalRotation();
    Vec3 dir = { 0.f, 0.f, 0.f };

    // WASD 입력에 따라 방향 벡터 설정
    if (INPUT->IsKeyPressed(DIK_W)) dir += GetTransform()->GetLook();
    if (INPUT->IsKeyPressed(DIK_S)) dir -= GetTransform()->GetLook();
    if (INPUT->IsKeyPressed(DIK_A)) dir -= GetTransform()->GetRight();
    if (INPUT->IsKeyPressed(DIK_D)) dir += GetTransform()->GetRight();

    if (dir.Length() > 0.f) {
        dir.Normalize();
        Vec3 look = GetTransform()->GetLook();
        look.Normalize();

        float dotProduct = std::clamp(look.Dot(dir), -1.0f, 1.0f);
        float dirAngle = acos(dotProduct);
        if (isnan(dirAngle)) dirAngle = XM_PI;

        Vec3 crossProduct = look.Cross(dir);
        if (crossProduct.Dot({ 0.f, 1.f, 0.f }) < 0) dirAngle = -dirAngle;

        _dir.y = revolution.y + dirAngle;

        if (parentRotate.y - _dir.y > XM_PI) parentRotate.y -= XM_2PI;
        if (parentRotate.y - _dir.y < -XM_PI) parentRotate.y += XM_2PI;
    }

    Vec3 result = { 0.f, _dir.y + 0.9f * (parentRotate.y - _dir.y), 0.f };
    parentTransform->SetLocalRotation(result);

    // Rigidbody 캐싱
    auto parentRigidbody = parent->GetRigidbody();

    // 광선 쏘기
   

    // WASD 이동 처리
    if (INPUT->IsKeyPressed(DIK_W) || INPUT->IsKeyPressed(DIK_S) ||
        INPUT->IsKeyPressed(DIK_A) || INPUT->IsKeyPressed(DIK_D)) {
        Vec3 lookVec = parentTransform->GetLook();
        parentRigidbody->SetVelocity(lookVec * _speed);

        if (INPUT->IsKeyPressed(DIK_SPACE)) {
            Vec3 upVec = parentTransform->GetUp();
            parentRigidbody->SetVelocity(upVec * _speed + parentRigidbody->GetVelocity());
        }
        if (INPUT->IsKeyPressed(DIK_LCONTROL)) {
            Vec3 downVec = -parentTransform->GetUp();
            parentRigidbody->SetVelocity(downVec * _speed + parentRigidbody->GetVelocity());
        }
    }

    // 스페이스바로 위쪽 이동, Ctrl로 아래쪽 이동
    else if (INPUT->IsKeyPressed(DIK_SPACE)) {
        Vec3 upVec = parentTransform->GetUp();
        parentRigidbody->SetVelocity(upVec * _speed);
    }
    else if (INPUT->IsKeyPressed(DIK_LCONTROL)) {
        Vec3 downVec = -parentTransform->GetUp();
        parentRigidbody->SetVelocity(downVec * _speed);
    }
    Vec4 rayDirection(parentRigidbody->GetVelocity().x, parentRigidbody->GetVelocity().y, parentRigidbody->GetVelocity().z, 0.0f);
    if (rayDirection.Length() <= 0.0f) {
        rayDirection = Vec4(parentTransform->GetLook().x, parentTransform->GetLook().y, parentTransform->GetLook().z, 0.0f);
        
    }
    rayDirection.Normalize();


    Vec3 localPosition = parentTransform->GetLocalPosition();
    Vec4 rayOrigin(localPosition.x, localPosition.y, localPosition.z, 1.0f);

    if (shared_ptr<BaseCollider> collider = parent->GetCollider()) {
        rayOrigin.x += collider->GetCenter().x;
        rayOrigin.y += collider->GetCenter().y;
        rayOrigin.z += collider->GetCenter().z;

        if (collider->GetColliderType() == ColliderType::Sphere) {
            rayOrigin += rayDirection * collider->GetRadius();
        }
    }
    float rayDistance = 10.0f;
    RaycastHit hitInfo;
    if (GET_SINGLETON(PhysicsManager)->Raycast(rayOrigin, rayDirection, rayDistance, &hitInfo)) {

        // 충돌이 예상됨 - 회피 벡터 계산
        Vec3 rayDir3(rayDirection.x, rayDirection.y, rayDirection.z);   // 물체의 진행 방향
        Vec3 hitNormal3(hitInfo.normal.x, hitInfo.normal.y, hitInfo.normal.z);  // 장애물의 법선 벡터

        // 회피 벡터와 현재 진행 벡터 혼합
        Vec3 crossProduct = rayDir3.Cross(hitNormal3);
        crossProduct.Normalize();
        Vec3 desiredVelocity = crossProduct * parentRigidbody->GetVelocity().Length();


        Vec3 steeringForce = (desiredVelocity - parentRigidbody->GetVelocity()) * 0.7f;  // 조종 힘 계산

        // 새 속도 적용
        parentRigidbody->SetVelocity(parentRigidbody->GetVelocity() + steeringForce);
    }
    // 마우스 휠로 줌 기능 추가 (부드러운 줌 적용)
    int mouseWheel = INPUT->GetMouseWheel();
    Vec3 vectorToOrigin = pos - Vec3(0.f, 0.f, 0.f);
    float distance = vectorToOrigin.Length();

    float zoomChange = mouseWheel * ZOOM_SPEED * DELTA_TIME;  // 부드러운 줌 변화량 계산
    distance = std::clamp(distance - zoomChange, MIN_ZOOM_DISTANCE, MAX_ZOOM_DISTANCE);

    // 부모 객체 위치 적용
    GetTransform()->SetLocalPosition(pos);
}
