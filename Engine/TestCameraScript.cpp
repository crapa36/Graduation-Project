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

    // 부모 오브젝트 가져오기
    auto parent = GetGameObject()->GetParent().lock();
    if (!parent) return; // 부모가 없으면 함수 종료
    auto parentTransform = parent->GetTransform();
    auto parentRigidbody = parent->GetRigidbody();
    if (!parentRigidbody) return; // 리지드바디가 없으면 함수 종료

    // 레이 캐스팅 설정
    constexpr int rayCount = 10;                  // 레이의 개수
    constexpr float rayDistance = 50.0f;          // 레이의 거리

    // Collider 정보 기반 파라미터 조정
    float radius = 0.0f;
    if (auto collider = parent->GetCollider()) {
        radius = collider->GetRadius();
    }

    // 오브젝트의 현재 속도 및 레이 방향 설정
    Vec4 velocity(parentRigidbody->GetVelocity().x, parentRigidbody->GetVelocity().y, parentRigidbody->GetVelocity().z, 0.0f);
    Vec4 rayDirection = velocity;

    // 속도가 거의 없는 경우 시선 방향 사용
    if (rayDirection.LengthSquared() < std::numeric_limits<float>::epsilon()) {
        Vec3 lookDirection = parentTransform->GetLook();
        rayDirection = Vec4(lookDirection.x, lookDirection.y, lookDirection.z, 0.0f);
    }
    rayDirection.Normalize();

    // 최적의 회피 방향 초기화
    Vec3 bestAvoidanceDirection(0.0f, 0.0f, 0.0f);
    float bestDotProduct = -1.0f; // 초기값은 최소값

    // 레이 캐스팅을 통한 장애물 감지 및 회피 방향 계산 (원형 패턴)
    for (int i = 0; i < rayCount; ++i) {

        // 원형 패턴에 따른 각도 계산 (균등 분포)
        float angle = (i * (360.0f / rayCount)) * (3.14159265f / 180.0f); // 각도를 라디안으로 변환

        // 레이의 시작 지점 계산 (원형 패턴)
        Vec4 rayOrigin(
            parentTransform->GetLocalPosition().x + cos(angle) * radius,
            parentTransform->GetLocalPosition().y,
            parentTransform->GetLocalPosition().z + sin(angle) * radius,
            1.0f
        );

        // Collider의 중심을 기준으로 레이 시작 위치 조정
        if (auto collider = parent->GetCollider()) {
            rayOrigin.x += collider->GetCenter().x;
            rayOrigin.y += collider->GetCenter().y;
            rayOrigin.z += collider->GetCenter().z;
            rayOrigin += rayDirection * 5.0f; // 오프셋을 고정값으로 설정 (필요에 따라 조정 가능)
        }

        // 레이 캐스팅 수행
        RaycastHit hitInfo;
        if (GET_SINGLETON(PhysicsManager)->Raycast(rayOrigin, rayDirection, rayDistance, &hitInfo, parent)) {

            // 회피 방향 계산: 레이 방향과 충돌 노멀의 반사를 이용
            Vec4 avoidanceDirection = rayDirection - 2.0f * rayDirection.Dot(hitInfo.normal) * hitInfo.normal;
            avoidanceDirection.Normalize();

            // 회피 방향과 레이 방향 간의 도트 프로덕트 계산 (절대값 사용)
            float dotProduct = std::abs(rayDirection.Dot(avoidanceDirection));

            // 최적의 회피 방향 선택 (가장 유사한 방향)
            if (dotProduct > bestDotProduct) {
                bestDotProduct = dotProduct;
                bestAvoidanceDirection = Vec3(avoidanceDirection.x, avoidanceDirection.y, avoidanceDirection.z);
            }
        }
    }

    // 최적의 회피 방향 적용
    if (bestDotProduct > -1.0f) { // 최소한 하나의 충돌이 감지된 경우
        bestAvoidanceDirection.Normalize();

        // 기존 속도의 크기를 유지하며 회피 방향으로 속도 설정
        Vec3 newVelocity = bestAvoidanceDirection * velocity.Length();

        // 속도를 직접 변경
        parentRigidbody->SetVelocity(newVelocity);
    }
}