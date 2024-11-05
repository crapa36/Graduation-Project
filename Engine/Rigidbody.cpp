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
        _velocity += Vec3(0.0f, -50.f, 0.0f) * DELTA_TIME;
    }

    // 마찰력 적용
    if (_isGrounded) {
        Vec3 horizontalVelocity = Vec3(_velocity.x, 0.0f, _velocity.z);
        float speed = horizontalVelocity.Length();
        if (speed > 0.0f) {
            float frictionForce = _friction * _mass * 50.f; // F_friction = μ * m * g
            Vec3 friction = -horizontalVelocity / speed * frictionForce * DELTA_TIME / _mass;
            Vec3 newHorizontalVelocity = horizontalVelocity + friction;

            // 속도가 0을 넘어 반대 방향으로 가지 않도록
            if (horizontalVelocity.Dot(newHorizontalVelocity) <= 0.0f) {
                newHorizontalVelocity = Vec3(0.0f, 0.0f, 0.0f);
            }

            _velocity.x = newHorizontalVelocity.x;
            _velocity.z = newHorizontalVelocity.z;
        }
    }

    // 선형 감쇠 적용
    _velocity *= max(0.0f, (1.0f - _drag * DELTA_TIME));

    // 각 감쇠 적용
    _angularVelocity *= max(0.0f, (1.0f - _angularDrag * DELTA_TIME));

    // 속도와 각속도가 일정 이하일시 정지로 처리
    if (_velocity.Length() < 0.01f) {
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