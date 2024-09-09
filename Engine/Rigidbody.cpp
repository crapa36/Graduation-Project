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

    // �߷��� �����մϴ�.
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.f, -9.8f, 0.f) * DELTA_TIME * 2;
    }

    // ���� ���踦 �����մϴ�.
    _velocity *= pow(1.0f - _drag, DELTA_TIME);

    // �� ���踦 �����մϴ�.
    _angularVelocity *= pow(1.0f - _angularDrag, DELTA_TIME);
    _isGrounded = false;

    //ĳ������ ������ �����մϴ�.
    if (_direction.y != GetTransform()->GetLocalRotation().y) { // ĳ������ ������ ��ǥ ����� �ٸ��ٸ�
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

    // �ӵ��� ���� ��ġ�� �����մϴ�.
    GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + _velocity * DELTA_TIME);

    // ���ӵ��� ���� ȸ���� �����մϴ�.
    GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(Vec3 force) {
    Vec3 acceleration = force / _mass;

    // ���ӵ��� ���� �ӵ��� ���Ͽ� ���ο� �ӵ��� ����մϴ�.
    _velocity += acceleration;
}

void Rigidbody::AddTorque(Vec3 torque) {

    // ��ũ�� �������� ������ �����ӵ��� ����մϴ�.
    Vec3 angularAcceleration = torque / _mass;

    // �����ӵ��� ���� ���ӵ��� ���Ͽ� ���ο� ���ӵ��� ����մϴ�.
    _angularVelocity += angularAcceleration;
}

void Rigidbody::OnCollisionEnter(const shared_ptr<GameObject>& other) {

    // TODO : �浹 ó�� �ڵ�

    Vec3 collisionNormal = GetCollider()->GetCollisionNormal(other->GetCollider());
    float collisionDepth = GetCollider()->GetCollisionDepth(other->GetCollider());

    // Vec3 ��ü�� �����Ͽ� Dot �޼��带 ȣ��
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = (_velocity - 2 * velocityDotNormal * collisionNormal) * _elasticity;

    AddForce(collisionNormal * collisionDepth * 2);

    if (other->GetRigidbody()) {
        float velocityDotNormal = _velocity.Dot(collisionNormal);
        AddForce(-collisionNormal * velocityDotNormal);
    }
    _isGrounded = true;
}