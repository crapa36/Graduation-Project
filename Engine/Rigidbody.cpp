#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Timer.h"
#include "BaseCollider.h"
#include "GameObject.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY) {

    // 초기화
    _mass = 1.0f;  // 기본 질량 설정
    _drag = 0.0f;  // 기본 선형 감쇠 설정
    _angularDrag = 0.0f;  // 기본 각 감쇠 설정
    _elasticity = 0.5f;  // 기본 탄성 계수 설정
    _useGravity = true;  // 기본 중력 사용 설정
    _velocity = Vec3(0.0f, 0.0f, 0.0f);
    _angularVelocity = Vec3(0.0f, 0.0f, 0.0f);
    _isGrounded = false;
}

Rigidbody::~Rigidbody() {
}

void Rigidbody::init() {

    // 초기화 코드 (필요 시)
}

void Rigidbody::Update() {

    // 중력 적용
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.0f, -50.f, 0.0f) * DELTA_TIME;
    }

    // 선형 감쇠 적용
    _velocity *= max(0.0f, (1.0f - _drag * DELTA_TIME));

    // 각 감쇠 적용
    _angularVelocity *= max(0.0f, (1.0f - _angularDrag * DELTA_TIME));

    // 속도와 각속도가 일정 이하일시 정지로 처리
    if (_velocity.Length() < 0.01) {
        _velocity = Vec3(0.0f, 0.0f, 0.0f);
    }

    _isGrounded = false;
}

void Rigidbody::LastUpdate() {
}

void Rigidbody::FinalUpdate() {

    // 속도에 따라 위치 변경
    auto transform = GetTransform();
    transform->SetLocalPosition(transform->GetLocalPosition() + _velocity * DELTA_TIME);

    // 각속도에 따라 회전 변경
    transform->SetLocalRotation(transform->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(const Vec3& force) {
    if (_mass > 0) {
        Vec3 acceleration = force / _mass;
        _velocity += acceleration * DELTA_TIME;  // 시간에 따른 힘의 효과 반영
    }
}

void Rigidbody::AddTorque(const Vec3& torque) {
    if (_mass > 0) {
        Vec3 angularAcceleration = torque / _mass;
        _angularVelocity += angularAcceleration * DELTA_TIME;  // 시간에 따른 토크의 효과 반영
    }
}