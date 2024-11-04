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

    // WASD �̵� ó��
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

    //AvoidObstaclesWithRays();

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
    // 부�?객체???�랜?�폼�?리�??�바?��? 가?�옴
    auto parent = GetGameObject()->GetParent().lock();
    auto parentTransform = parent->GetTransform();
    auto parentRigidbody = parent->GetRigidbody();

    // ?�피 ?�고리즘 ?�정
    constexpr int rayCount = 10;                // 발사??광선 ??
    constexpr float rayDistance = 10.0f;        // 광선 거리
    constexpr float baseAvoidanceStrength = 1.0f; // 기본 ?�피 가중치

    // Collider??반경???�용???�전 반경�?각도 ?�정
    float spiralRadiusIncrement = 1.0f;         // 기본 반경 증�?�?
    float spiralAngleIncrement = 30.0f;         // 기본 각도 증�?�?
    if (auto collider = parent->GetCollider()) {
        float radius = collider->GetRadius();
        spiralRadiusIncrement = radius * 0.1f;          // 반경??Collider 반경??10%�?증�?
        spiralAngleIncrement = 360.0f / rayCount;       // ?�체 360?��? rayCount만큼 균등?�게 ?�눔
    }

    // 객체???�도?� 광선 방향 ?�정 (Vec4�??��????��?)
    Vec4 velocity(parentRigidbody->GetVelocity().x, parentRigidbody->GetVelocity().y, parentRigidbody->GetVelocity().z, 0.0f);
    Vec4 rayDirection = velocity;

    // 객체가 ?�직이지 ?�을 ?�는 ?�재 바라보는 방향???�용
    if (rayDirection.LengthSquared() < std::numeric_limits<float>::epsilon()) {
        Vec3 lookDirection = parentTransform->GetLook();
        rayDirection = Vec4(lookDirection.x, lookDirection.y, lookDirection.z, 0.0f);
    }
    rayDirection.Normalize();

    // 가??최적???�피 방향??추적??변?�들
    Vec3 bestAvoidanceDirection(0.0f, 0.0f, 0.0f);
    float bestDotProduct = -1.0f;  // ?�동 방향�??�피 벡터 간의 ?�사?�을 비교?�기 ?�한 �?

    // ?�선?�으�?광선??발사
    for (int i = 0; i < rayCount; ++i) {
        // �?광선???�전 각도?� 반경 ?�정
        float angle = i * spiralAngleIncrement * (3.14159265f / 180.0f); // ?�디???�위�?변??
        float radius = i * spiralRadiusIncrement;

        // ?�선??광선 ?�작 ?�치 계산 (x, z ?�면???�선?�으�?배치)
        Vec4 rayOrigin(
            parentTransform->GetLocalPosition().x + cos(angle) * radius,
            parentTransform->GetLocalPosition().y,
            parentTransform->GetLocalPosition().z + sin(angle) * radius,
            1.0f
        );

        // Collider가 ?�을 경우 광선 ?�작 ?�치 보정
        if (auto collider = parent->GetCollider()) {
            rayOrigin.x += collider->GetCenter().x;
            rayOrigin.y += collider->GetCenter().y;
            rayOrigin.z += collider->GetCenter().z;
            rayOrigin += rayDirection * collider->GetRadius();
        }

        // 광선??발사?�여 충돌 감�?
        RaycastHit hitInfo;
        if (GET_SINGLETON(PhysicsManager)->Raycast(rayOrigin, rayDirection, rayDistance, &hitInfo)) {
            Vec3 hitNormal(hitInfo.normal.x, hitInfo.normal.y, hitInfo.normal.z);

            // 법선 벡터?� 광선 방향??직교 벡터�??�피 방향 계산
            Vec3 avoidanceDirection = Vec3(rayDirection.x, rayDirection.y, rayDirection.z).Cross(hitNormal);
            avoidanceDirection.Normalize();

            // ?�재 ?�동 방향�??�피 벡터???�사??Dot Product) 계산
            float dotProduct = Vec3(rayDirection.x, rayDirection.y, rayDirection.z).Dot(avoidanceDirection);

            // ?�동 벡터?� 가???�사???�피 벡터�??�택
            if (dotProduct > bestDotProduct) {
                bestDotProduct = dotProduct;
                bestAvoidanceDirection = avoidanceDirection;
            }
        }
    }

    // 가???�사???�피 벡터�??�동 방향??강제 ?�환
    if (bestDotProduct > -1.0f) { // ?�효???�피 벡터가 ?�는 경우
        bestAvoidanceDirection.Normalize();
        parentRigidbody->SetVelocity(bestAvoidanceDirection * velocity.Length()); // 기존 ?�도???�기 ?��?
    }
}
