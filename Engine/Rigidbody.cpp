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
    if (_useGravity) {
        AddForce(Vec4(0, -9.8f, 0, 0));
    }

    // ���� ���踦 �����մϴ�.
    _velocity *= (1.0f - _drag);

    // �� ���踦 �����մϴ�.
    _angularVelocity *= (1.0f - _angularDrag);
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

    _velocity = Vec4(0, 0, 0, 0);
}