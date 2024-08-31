#pragma once
#include "Component.h"

class Rigidbody : public Component {
public:
    Rigidbody();
    virtual ~Rigidbody();
    void init();

    void Update();
    void LastUpdate();
    void FinalUpdate();

    void AddForce(Vec3 force);
    void AddTorque(Vec3 torque);

    void SetMass(float mass) { _mass = mass; }
    void SetDrag(float drag) { _drag = drag; }
    void SetAngularDrag(float angularDrag) { _angularDrag = angularDrag; }
    void SetUseGravity(bool useGravity) { _useGravity = useGravity; }
    void SetIsKinematic(bool isKinematic) { _isKinematic = isKinematic; }

    Vec3 GetVelocity() { return _velocity; }
    Vec3 GetAngularVelocity() { return _angularVelocity; }

    float GetMass() { return _mass; }
    float GetDrag() { return _drag; }
    float GetAngularDrag() { return _angularDrag; }
    bool GetUseGravity() { return _useGravity; }
    bool GetIsKinematic() { return _isKinematic; }

    void OnCollisionEnter(const shared_ptr<class GameObject>& other); // 함수 선언 수정

private:
    float _mass = 0.5f;
    float _drag = 0.05f;
    float _angularDrag = 0.05f;
    bool _useGravity = true;
    bool _isKinematic = false;
    bool _isGrounded = false;

    float _elasticity = 0.9f; // 기본 탄성 계수

    Vec3 _velocity = Vec3(0.f, 0.f, 0.f);
    Vec3 _angularVelocity = Vec3(0.f, 0.f, 0.f);
};
