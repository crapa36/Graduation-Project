#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Timer.h"
#include "BaseCollider.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY) {
}

Rigidbody::~Rigidbody() {
}

void Rigidbody::init() {
}

void Rigidbody::Update() {

    // 중력을 적용합니다.
    if (_useGravity) {
        AddForce(Vec4(0, -9.8f, 0, 0));
    }

    // 선형 감쇠를 적용합니다.
    _velocity *= (1.0f - _drag);

    // 각 감쇠를 적용합니다.
    _angularVelocity *= (1.0f - _angularDrag);
}

void Rigidbody::LastUpdate() {
}

void Rigidbody::FinalUpdate() {

    // 속도에 따라 위치를 변경합니다.
    GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + _velocity * DELTA_TIME);

    // 각속도에 따라 회전을 변경합니다.
    GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(Vec4 force) {
    Vec4 acceleration = force / _mass;

    // 가속도를 현재 속도에 더하여 새로운 속도를 계산합니다.
    _velocity += acceleration;
}

void Rigidbody::AddTorque(Vec4 torque) {

    // 토크를 질량으로 나누어 각가속도를 계산합니다.
    Vec4 angularAcceleration = torque / _mass;

    // 각가속도를 현재 각속도에 더하여 새로운 각속도를 계산합니다.
    _angularVelocity += angularAcceleration;
}
void Rigidbody::OnCollisionEnter(const shared_ptr<BaseCollider>& other) {

    // TODO : 충돌 처리 코드

    _velocity = Vec4(0, 0, 0, 0);
}