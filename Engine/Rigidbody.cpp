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

    // �߷��� �����մϴ�.
    if (_useGravity && !_isGrounded) {
        _velocity += Vec4(0.f, -9.8f, 0.f, 0.f) * DELTA_TIME * 2;
    }

    // ���� ���踦 �����մϴ�.
    _velocity *= pow(1.0f - _drag, DELTA_TIME);

    // �� ���踦 �����մϴ�.
    _angularVelocity *= pow(1.0f - _angularDrag, DELTA_TIME);
    _isGrounded = false;
}

void Rigidbody::LastUpdate() {
}

void Rigidbody::FinalUpdate() {

    // �ӵ��� ���� ��ġ�� �����մϴ�.

    GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + _velocity * DELTA_TIME);

    // ���ӵ��� ���� ȸ���� �����մϴ�.
    GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(Vec4 force) {
    Vec4 acceleration = force / _mass;

    // ���ӵ��� ���� �ӵ��� ���Ͽ� ���ο� �ӵ��� ����մϴ�.
    _velocity += acceleration;
}

void Rigidbody::AddTorque(Vec4 torque) {

    // ��ũ�� �������� ������ �����ӵ��� ����մϴ�.
    Vec4 angularAcceleration = torque / _mass;

    // �����ӵ��� ���� ���ӵ��� ���Ͽ� ���ο� ���ӵ��� ����մϴ�.
    _angularVelocity += angularAcceleration;
}
void Rigidbody::OnCollisionEnter(const shared_ptr<BaseCollider>& other) {

    // TODO : �浹 ó�� �ڵ�

    Vec4 collisionNormal = GetCollider()->GetCollisionNormal(other);
    float collisionDepth = GetCollider()->GetCollisionDepth(other);
    _velocity *= _elasticity;

    // Vec4 ��ü�� �����Ͽ� Dot �޼��带 ȣ��
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = _velocity - 2 * velocityDotNormal * collisionNormal;

    AddForce(collisionNormal * collisionDepth);
    _isGrounded = true;
}