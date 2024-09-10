#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Timer.h"
#include "BaseCollider.h"
#include "GameObject.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY), _velocity(0.0f, 0.0f, 0.0f), _angularVelocity(0.0f, 0.0f, 0.0f), _mass(1.0f), _elasticity(0.5f), _drag(0.1f), _angularDrag(0.1f), _useGravity(true), _isGrounded(false) {
}

Rigidbody::~Rigidbody() {
}

void Rigidbody::init() {
    // 초기화 코드 (필요 시)
}

void Rigidbody::Update() {
    // 중력을 적용합니다.
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.0f, -40.f, 0.0f) * DELTA_TIME;
    }

    // 선형 감쇠를 적용합니다.
    _velocity *= (1.0f - _drag * DELTA_TIME);

    // 각 감쇠를 적용합니다.
    _angularVelocity *= (1.0f - _angularDrag * DELTA_TIME);

    _isGrounded = false;
}

void Rigidbody::LastUpdate() {
    // 현재는 구현하지 않음
}

void Rigidbody::FinalUpdate() {
    // 속도에 따라 위치를 변경합니다.
    auto transform = GetTransform();
    transform->SetLocalPosition(transform->GetLocalPosition() + _velocity * DELTA_TIME);

    // 각속도에 따라 회전을 변경합니다.
    transform->SetLocalRotation(transform->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(const Vec3& force) {
    if (_mass > 0) {
        Vec3 acceleration = force / _mass;
        _velocity += acceleration * DELTA_TIME;  // 시간에 따른 힘의 효과를 반영
    }
}

void Rigidbody::AddTorque(const Vec3& torque) {
    if (_mass > 0) {
        Vec3 angularAcceleration = torque / _mass;
        _angularVelocity += angularAcceleration * DELTA_TIME;  // 시간에 따른 토크의 효과를 반영
    }
}

void Rigidbody::OnCollisionEnter(const shared_ptr<GameObject>& other) {
    if (!GetCollider() || !other->GetCollider()) {
        return;
    }

    Vec3 collisionNormal = GetCollider()->GetCollisionNormal(other->GetCollider());
    float collisionDepth = GetCollider()->GetCollisionDepth(other->GetCollider());

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
