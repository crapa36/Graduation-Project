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
        _velocity += Vec3(0.0f, -9.8f, 0.0f) * DELTA_TIME;  // 지구 중력 상수 사용
    }

    // 선형 감쇠 적용
    _velocity *= (1.0f - _drag * DELTA_TIME);

    // 각 감쇠 적용
    _angularVelocity *= (1.0f - _angularDrag * DELTA_TIME);

    _isGrounded = false;

    //캐릭터의 방향을 조정합니다.
    if (_direction.y != GetTransform()->GetLocalRotation().y) { // 캐릭터의 방향이 목표 방향과 다르다면
        if (_direction.y - GetTransform()->GetLocalRotation().y > 0) {
            _angularVelocity = { 0.f, 0.01f, 0.f };
        }
        else {
            _angularVelocity = { 0.f, -0.01f, 0.f };
        }
    }
    else {
        _angularVelocity = { 0.f, 0.f, 0.f };
    }
}

void Rigidbody::LastUpdate() {

    // 현재는 구현하지 않음
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

void Rigidbody::OnCollisionEnter(const std::shared_ptr<GameObject>& other, const Vec3 collisionNormal, const float collisionDepth) {
    if (!GetCollider() || !other->GetCollider()) {
        return;
    }

    // 물리 반응 적용
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = (_velocity - 2.0f * velocityDotNormal * collisionNormal) * _elasticity;

    // 충돌 깊이에 따른 힘 적용
    AddForce(collisionNormal * collisionDepth * 2.0f);

    if (auto otherRigidbody = other->GetRigidbody()) {
        float otherVelocityDotNormal = otherRigidbody->_velocity.Dot(collisionNormal);
        otherRigidbody->_velocity = (otherRigidbody->_velocity - 2.0f * otherVelocityDotNormal * collisionNormal) * otherRigidbody->_elasticity;
        otherRigidbody->AddForce(-collisionNormal * otherVelocityDotNormal);
    }

    _isGrounded = true;
}