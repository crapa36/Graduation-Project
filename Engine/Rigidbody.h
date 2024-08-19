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

    void AddForce(Vec4 force);
    void AddTorque(Vec4 torque);

    void SetMass(float mass) { _mass = mass; }
    void SetDrag(float drag) { _drag = drag; }
    void SetAngularDrag(float angularDrag) { _angularDrag = angularDrag; }
    void SetUseGravity(bool useGravity) { _useGravity = useGravity; }
    void SetIsKinematic(bool isKinematic) { _isKinematic = isKinematic; }

    Vec4 GetVelocity() { return _velocity; }
    Vec4 GetAngularVelocity() { return _angularVelocity; }

    float GetMass() { return _mass; }
    float GetDrag() { return _drag; }
    float GetAngularDrag() { return _angularDrag; }
    bool GetUseGravity() { return _useGravity; }
    bool GetIsKinematic() { return _isKinematic; }

    void OnCollisionEnter(const shared_ptr<class BaseCollider>& other); // 함수 선언 수정

private:
    float _mass = 1.f;
    float _drag = 0.0005f;
    float _angularDrag = 0.05f;
    bool _useGravity = true;
    bool _isKinematic = false;

    Vec4 _velocity = Vec4(0.f, 0.f, 0.f, 0.f);
    Vec4 _angularVelocity = Vec4(0.f, 0.f, 0.f, 0.f);
};
