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
}

void Rigidbody::Update() {

    // 중력을 적용합니다.
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.f, -9.8f, 0.f) * DELTA_TIME * 2;
    }

    // 선형 감쇠를 적용합니다.
    _velocity *= pow(1.0f - _drag, DELTA_TIME);

    // 각 감쇠를 적용합니다.
    _angularVelocity *= pow(1.0f - _angularDrag, DELTA_TIME);
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
}

void Rigidbody::FinalUpdate() {

    // 속도에 따라 위치를 변경합니다.
    GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + _velocity * DELTA_TIME);

    // 각속도에 따라 회전을 변경합니다.
    GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(Vec3 force) {
    Vec3 acceleration = force / _mass;

    // 가속도를 현재 속도에 더하여 새로운 속도를 계산합니다.
    _velocity += acceleration;
}

void Rigidbody::AddTorque(Vec3 torque) {

    // 토크를 질량으로 나누어 각가속도를 계산합니다.
    Vec3 angularAcceleration = torque / _mass;

    // 각가속도를 현재 각속도에 더하여 새로운 각속도를 계산합니다.
    _angularVelocity += angularAcceleration;
}

void Rigidbody::OnCollisionEnter(const shared_ptr<GameObject>& other) {

    // TODO : 충돌 처리 코드

    Vec3 collisionNormal = GetCollider()->GetCollisionNormal(other->GetCollider());
    float collisionDepth = GetCollider()->GetCollisionDepth(other->GetCollider());

    // Vec3 객체를 생성하여 Dot 메서드를 호출
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = (_velocity - 2 * velocityDotNormal * collisionNormal) * _elasticity;

    AddForce(collisionNormal * collisionDepth * 2);

    if (other->GetRigidbody()) {
        float velocityDotNormal = _velocity.Dot(collisionNormal);
        AddForce(-collisionNormal * velocityDotNormal);
    }
    _isGrounded = true;
}