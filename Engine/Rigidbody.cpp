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
    // �ʱ�ȭ �ڵ� (�ʿ� ��)
}

void Rigidbody::Update() {
    // �߷��� �����մϴ�.
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.0f, -40.f, 0.0f) * DELTA_TIME;
    }

    // ���� ���踦 �����մϴ�.
    _velocity *= (1.0f - _drag * DELTA_TIME);

    // �� ���踦 �����մϴ�.
    _angularVelocity *= (1.0f - _angularDrag * DELTA_TIME);

    _isGrounded = false;
}

void Rigidbody::LastUpdate() {
    // ����� �������� ����
}

void Rigidbody::FinalUpdate() {
    // �ӵ��� ���� ��ġ�� �����մϴ�.
    auto transform = GetTransform();
    transform->SetLocalPosition(transform->GetLocalPosition() + _velocity * DELTA_TIME);

    // ���ӵ��� ���� ȸ���� �����մϴ�.
    transform->SetLocalRotation(transform->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(const Vec3& force) {
    if (_mass > 0) {
        Vec3 acceleration = force / _mass;
        _velocity += acceleration * DELTA_TIME;  // �ð��� ���� ���� ȿ���� �ݿ�
    }
}

void Rigidbody::AddTorque(const Vec3& torque) {
    if (_mass > 0) {
        Vec3 angularAcceleration = torque / _mass;
        _angularVelocity += angularAcceleration * DELTA_TIME;  // �ð��� ���� ��ũ�� ȿ���� �ݿ�
    }
}

void Rigidbody::OnCollisionEnter(const shared_ptr<GameObject>& other) {
    if (!GetCollider() || !other->GetCollider()) {
        return;
    }

    Vec3 collisionNormal = GetCollider()->GetCollisionNormal(other->GetCollider());
    float collisionDepth = GetCollider()->GetCollisionDepth(other->GetCollider());

    // ���� ���� ����
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = (_velocity - 2.0f * velocityDotNormal * collisionNormal) * _elasticity;

    // �浹 ���̿� ���� �� ����
    AddForce(collisionNormal * collisionDepth * 2.0f);

    if (auto otherRigidbody = other->GetRigidbody()) {
        float otherVelocityDotNormal = otherRigidbody->_velocity.Dot(collisionNormal);
        otherRigidbody->_velocity = (otherRigidbody->_velocity - 2.0f * otherVelocityDotNormal * collisionNormal) * otherRigidbody->_elasticity;
        otherRigidbody->AddForce(-collisionNormal * otherVelocityDotNormal);
    }

    _isGrounded = true;
}
