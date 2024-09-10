#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Timer.h"
#include "BaseCollider.h"
#include "GameObject.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY) {

    // �ʱ�ȭ
    _mass = 1.0f;  // �⺻ ���� ����
    _drag = 0.0f;  // �⺻ ���� ���� ����
    _angularDrag = 0.0f;  // �⺻ �� ���� ����
    _elasticity = 0.5f;  // �⺻ ź�� ��� ����
    _useGravity = true;  // �⺻ �߷� ��� ����
    _velocity = Vec3(0.0f, 0.0f, 0.0f);
    _angularVelocity = Vec3(0.0f, 0.0f, 0.0f);
    _isGrounded = false;
}

Rigidbody::~Rigidbody() {
}

void Rigidbody::init() {

    // �ʱ�ȭ �ڵ� (�ʿ� ��)
}

void Rigidbody::Update() {

    // �߷� ����
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.0f, -9.8f, 0.0f) * DELTA_TIME;  // ���� �߷� ��� ���
    }

    // ���� ���� ����
    _velocity *= (1.0f - _drag * DELTA_TIME);

    // �� ���� ����
    _angularVelocity *= (1.0f - _angularDrag * DELTA_TIME);

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

    // ����� �������� ����
}

void Rigidbody::FinalUpdate() {

    // �ӵ��� ���� ��ġ ����
    auto transform = GetTransform();
    transform->SetLocalPosition(transform->GetLocalPosition() + _velocity * DELTA_TIME);

    // ���ӵ��� ���� ȸ�� ����
    transform->SetLocalRotation(transform->GetLocalRotation() + _angularVelocity * DELTA_TIME);
}

void Rigidbody::AddForce(const Vec3& force) {
    if (_mass > 0) {
        Vec3 acceleration = force / _mass;
        _velocity += acceleration * DELTA_TIME;  // �ð��� ���� ���� ȿ�� �ݿ�
    }
}

void Rigidbody::AddTorque(const Vec3& torque) {
    if (_mass > 0) {
        Vec3 angularAcceleration = torque / _mass;
        _angularVelocity += angularAcceleration * DELTA_TIME;  // �ð��� ���� ��ũ�� ȿ�� �ݿ�
    }
}

void Rigidbody::OnCollisionEnter(const std::shared_ptr<GameObject>& other, const Vec3 collisionNormal, const float collisionDepth) {
    if (!GetCollider() || !other->GetCollider()) {
        return;
    }

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