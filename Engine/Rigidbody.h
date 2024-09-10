#pragma once
#include "Component.h"

class Rigidbody : public Component {
public:
    Rigidbody();
    virtual ~Rigidbody();

    // �ʱ�ȭ �� ������Ʈ �޼ҵ�
    void init();
    void Update();
    void LastUpdate();
    void FinalUpdate();

    // ���� �Ӽ� ����
    void AddForce(const Vec3& force);
    void AddTorque(const Vec3& torque);

    void OnCollisionEnter(const std::shared_ptr<GameObject>& other, const Vec3 collisionNormal, const float collisionDepth);

    void SetMass(float mass) { _mass = mass; }
    void SetDrag(float drag) { _drag = drag; }
    void SetAngularDrag(float angularDrag) { _angularDrag = angularDrag; }
    void SetUseGravity(bool useGravity) { _useGravity = useGravity; }
    void SetIsKinematic(bool isKinematic) { _isKinematic = isKinematic; }
    void SetElasticity(float elasticity) { _elasticity = elasticity; }

    Vec3 GetVelocity() const { return _velocity; }
    Vec3 GetAngularVelocity() const { return _angularVelocity; }

    float GetMass() const { return _mass; }
    float GetDrag() const { return _drag; }
    float GetAngularDrag() const { return _angularDrag; }
    bool GetUseGravity() const { return _useGravity; }
    bool GetIsKinematic() const { return _isKinematic; }

private:

    // ���� �Ӽ�
    float _mass = 0.5f;               // ������ٵ��� ����
    float _drag = 0.01f;              // ���� �巡�� ���
    float _angularDrag = 0.01f;      // ���ӵ� �巡�� ���
    float _elasticity = 0.9f;        // ������ٵ��� ź�� ��� (�ݹ߷�)

    bool _useGravity = true;         // �߷��� ������� ����
    bool _isKinematic = false;       // ���� ������ ������ ���� �ʴ� Ű�׸�ƽ ����

    bool _isGrounded = false;        // ��ü�� ���鿡 ��� �ִ��� ����

    Vec3 _velocity = Vec3(0.f, 0.f, 0.f);          // ���� �ӵ�
    Vec3 _angularVelocity = Vec3(0.f, 0.f, 0.f);   // ���ӵ�
};
