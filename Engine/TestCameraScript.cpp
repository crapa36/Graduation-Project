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
#include <cmath>

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

TestCameraScript::~TestCameraScript() {}

void TestCameraScript::LateUpdate() {
    if (GetForegroundWindow() != GEngine->GetWindow().hwnd) return;

    Vec3 revolution = GetTransform()->GetLocalRevolution();
    Vec3 pos = GetTransform()->GetLocalPosition();

    if (INPUT->IsMouseButtonPressed(0)) {
        const POINT& mousePos = INPUT->GetMousePos();
        GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
    }

    _isMouseLock = INPUT->IsKeyPressed(DIK_LALT) ? false : true;

    if (_isMouseLock) {
        INPUT->SetCurrentCursor(L"null");
        ShowCursor(false);
        const POINT& currentMousePos = INPUT->GetMousePos();

        float deltaX = static_cast<float>(currentMousePos.x - _centerPos.x);
        float deltaY = static_cast<float>(currentMousePos.y - _centerPos.y);

        revolution.y += deltaX * SENSITIVITY;
        revolution.x += deltaY * SENSITIVITY;

        SetCursorPos(_centerScreenPos.x, _centerScreenPos.y);
    }
    else {
        INPUT->SetCurrentCursor(L"Arrow");
        ShowCursor(true);
    }

    GetTransform()->SetLocalRevolution(revolution);

    float targetSpeed = INPUT->IsKeyPressed(DIK_LSHIFT) ? BOOSTED_SPEED : DEFAULT_SPEED;
    _speed += (targetSpeed - _speed) * DELTA_TIME * 5.0f;

    shared_ptr<GameObject> parent = GetGameObject()->GetParent().lock();
    shared_ptr<Transform> parentTransform = parent->GetTransform();
    Vec3 parentRotate = parentTransform->GetLocalRotation();
    Vec3 dir = { 0.f, 0.f, 0.f };

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

    auto parentRigidbody = parent->GetRigidbody();

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
    else if (INPUT->IsKeyPressed(DIK_SPACE)) {
        Vec3 upVec = parentTransform->GetUp();
        parentRigidbody->SetVelocity(upVec * _speed);
    }
    else if (INPUT->IsKeyPressed(DIK_LCONTROL)) {
        Vec3 downVec = -parentTransform->GetUp();
        parentRigidbody->SetVelocity(downVec * _speed);
    }
    AvoidObstaclesWithRays();

    int mouseWheel = INPUT->GetMouseWheel();
    Vec3 vectorToOrigin = pos - Vec3(0.f, 0.f, 0.f);
    float distance = vectorToOrigin.Length();

    float zoomChange = mouseWheel * ZOOM_SPEED * DELTA_TIME;
    distance = std::clamp(distance - zoomChange, MIN_ZOOM_DISTANCE, MAX_ZOOM_DISTANCE);

    GetTransform()->SetLocalPosition(pos);
}

Vec4 RotateVector(const Vec4& vector, float angleDegrees) {
    float angleRadians = angleDegrees * (3.14f / 180.0f);
    float cosAngle = cos(angleRadians);
    float sinAngle = sin(angleRadians);

    Vec4 rotatedVector;
    rotatedVector.x = vector.x * cosAngle - vector.z * sinAngle;
    rotatedVector.y = vector.y;
    rotatedVector.z = vector.x * sinAngle + vector.z * cosAngle;
    rotatedVector.w = vector.w;

    return rotatedVector;
}

void TestCameraScript::AvoidObstaclesWithRays() {
    // 부모 객체의 트랜스폼과 리지드바디를 가져옴
    auto parent = GetGameObject()->GetParent().lock();
    auto parentTransform = parent->GetTransform();
    auto parentRigidbody = parent->GetRigidbody();

    // 회피 알고리즘 설정
    constexpr int rayCount = 10;                // 발사할 광선 수
    constexpr float rayDistance = 10.0f;        // 광선 거리
    constexpr float baseAvoidanceStrength = 1.0f; // 기본 회피 가중치

    // Collider의 반경을 이용해 회전 반경과 각도 설정
    float spiralRadiusIncrement = 1.0f;         // 기본 반경 증가값
    float spiralAngleIncrement = 30.0f;         // 기본 각도 증가값
    if (auto collider = parent->GetCollider()) {
        float radius = collider->GetRadius();
        spiralRadiusIncrement = radius * 0.1f;          // 반경을 Collider 반경의 10%로 증가
        spiralAngleIncrement = 360.0f / rayCount;       // 전체 360도를 rayCount만큼 균등하게 나눔
    }

    // 객체의 속도와 광선 방향 설정 (Vec4로 일관성 유지)
    Vec4 velocity(parentRigidbody->GetVelocity().x, parentRigidbody->GetVelocity().y, parentRigidbody->GetVelocity().z, 0.0f);
    Vec4 rayDirection = velocity;

    // 객체가 움직이지 않을 때는 현재 바라보는 방향을 사용
    if (rayDirection.LengthSquared() < std::numeric_limits<float>::epsilon()) {
        Vec3 lookDirection = parentTransform->GetLook();
        rayDirection = Vec4(lookDirection.x, lookDirection.y, lookDirection.z, 0.0f);
    }
    rayDirection.Normalize();

    // 가장 최적의 회피 방향을 추적할 변수들
    Vec3 bestAvoidanceDirection(0.0f, 0.0f, 0.0f);
    float bestDotProduct = -1.0f;  // 이동 방향과 회피 벡터 간의 유사성을 비교하기 위한 값

    // 나선형으로 광선을 발사
    for (int i = 0; i < rayCount; ++i) {
        // 각 광선의 회전 각도와 반경 설정
        float angle = i * spiralAngleIncrement * (3.14159265f / 180.0f); // 라디안 단위로 변환
        float radius = i * spiralRadiusIncrement;

        // 나선형 광선 시작 위치 계산 (x, z 평면에 나선형으로 배치)
        Vec4 rayOrigin(
            parentTransform->GetLocalPosition().x + cos(angle) * radius,
            parentTransform->GetLocalPosition().y,
            parentTransform->GetLocalPosition().z + sin(angle) * radius,
            1.0f
        );

        // Collider가 있을 경우 광선 시작 위치 보정
        if (auto collider = parent->GetCollider()) {
            rayOrigin.x += collider->GetCenter().x;
            rayOrigin.y += collider->GetCenter().y;
            rayOrigin.z += collider->GetCenter().z;
            rayOrigin += rayDirection * collider->GetRadius();
        }

        // 광선을 발사하여 충돌 감지
        RaycastHit hitInfo;
        if (GET_SINGLETON(PhysicsManager)->Raycast(rayOrigin, rayDirection, rayDistance, &hitInfo)) {
            Vec3 hitNormal(hitInfo.normal.x, hitInfo.normal.y, hitInfo.normal.z);

            // 법선 벡터와 광선 방향의 직교 벡터로 회피 방향 계산
            Vec3 avoidanceDirection = Vec3(rayDirection.x, rayDirection.y, rayDirection.z).Cross(hitNormal);
            avoidanceDirection.Normalize();

            // 현재 이동 방향과 회피 벡터의 유사성(Dot Product) 계산
            float dotProduct = Vec3(rayDirection.x, rayDirection.y, rayDirection.z).Dot(avoidanceDirection);

            // 이동 벡터와 가장 유사한 회피 벡터를 선택
            if (dotProduct > bestDotProduct) {
                bestDotProduct = dotProduct;
                bestAvoidanceDirection = avoidanceDirection;
            }
        }
    }

    // 가장 유사한 회피 벡터로 이동 방향을 강제 전환
    if (bestDotProduct > -1.0f) { // 유효한 회피 벡터가 있는 경우
        bestAvoidanceDirection.Normalize();
        parentRigidbody->SetVelocity(bestAvoidanceDirection * velocity.Length()); // 기존 속도의 크기 유지
    }
}
