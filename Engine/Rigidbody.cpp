#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Timer.h"
#include "BaseCollider.h"
#include "GameObject.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY) {

}

Rigidbody::~Rigidbody() {
}

void Rigidbody::init() {

    // 초기화 코드 (필요 시)
}

void Rigidbody::Update() {
    // 중력 적용
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.0f, -20.0f, 0.0f) * DELTA_TIME;
    }

    // 선형 감쇠 적용
    _velocity *= (1.0f - _drag * DELTA_TIME);

    // 각 감쇠 적용
    _angularVelocity *= (1.0f - _angularDrag * DELTA_TIME);

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

void Rigidbody::OnCollisionEnter(const std::shared_ptr<GameObject>& other, Vec3 collisionNormal, float collisionDepth) {
    if (!GetCollider() || !other->GetCollider()) {
        return;
    }

    // 물리 반응 적용 (자신의 속도 반영)
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = -(_velocity - 2.0f * velocityDotNormal * collisionNormal) * _elasticity;

    // 충돌 깊이에 따른 힘 적용

    AddForce(collisionNormal * collisionDepth * 2.0);

    // 상대방이 리지드바디를 가지고 있다면
    if (other->GetRigidbody()) {
        Vec3 otherVelocity = other->GetRigidbody()->GetVelocity();

        // 상대 속도 계산
        Vec3 relativeVelocity = otherVelocity - _velocity;
        float relativeVelocityDotNormal = relativeVelocity.Dot(collisionNormal);

        // 상대 속도를 기반으로 힘 적용
        AddForce(collisionNormal * relativeVelocityDotNormal);
    }

    _isGrounded = true;
}

