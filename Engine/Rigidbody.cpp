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

    // �ʱ�ȭ �ڵ� (�ʿ� ��)
}

void Rigidbody::Update() {
    // �߷� ����
    if (_useGravity && !_isGrounded) {
        _velocity += Vec3(0.0f, -20.0f, 0.0f) * DELTA_TIME;
    }

    // ���� ���� ����
    _velocity *= (1.0f - _drag * DELTA_TIME);

    // �� ���� ����
    _angularVelocity *= (1.0f - _angularDrag * DELTA_TIME);

    _isGrounded = false;
   
}

void Rigidbody::LastUpdate() {
   
   
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

void Rigidbody::OnCollisionEnter(const std::shared_ptr<GameObject>& other, Vec3 collisionNormal, float collisionDepth) {
    if (!GetCollider() || !other->GetCollider()) {
        return;
    }

    // ���� ���� ���� (�ڽ��� �ӵ� �ݿ�)
    float velocityDotNormal = _velocity.Dot(collisionNormal);
    _velocity = -(_velocity - 2.0f * velocityDotNormal * collisionNormal) * _elasticity;

    // �浹 ���̿� ���� �� ����

    AddForce(collisionNormal * collisionDepth * 2.0);

    // ������ ������ٵ� ������ �ִٸ�
    if (other->GetRigidbody()) {
        Vec3 otherVelocity = other->GetRigidbody()->GetVelocity();

        // ��� �ӵ� ���
        Vec3 relativeVelocity = otherVelocity - _velocity;
        float relativeVelocityDotNormal = relativeVelocity.Dot(collisionNormal);

        // ��� �ӵ��� ������� �� ����
        AddForce(collisionNormal * relativeVelocityDotNormal);
    }

    _isGrounded = true;
}

