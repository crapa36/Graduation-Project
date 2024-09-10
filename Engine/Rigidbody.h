#pragma once
#include "Component.h"

class Rigidbody : public Component {
public:
    Rigidbody();
    virtual ~Rigidbody();

    // 초기화 및 업데이트 메소드
    void init();
    void Update();
    void LastUpdate();
    void FinalUpdate();

    // 물리 속성 관리
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

    // 물리 속성
    float _mass = 0.5f;               // 리지드바디의 질량
    float _drag = 0.01f;              // 선형 드래그 계수
    float _angularDrag = 0.01f;      // 각속도 드래그 계수
    float _elasticity = 0.9f;        // 리지드바디의 탄성 계수 (반발력)

    bool _useGravity = true;         // 중력을 사용할지 여부
    bool _isKinematic = false;       // 물리 엔진에 영향을 받지 않는 키네마틱 여부

    bool _isGrounded = false;        // 물체가 지면에 닿아 있는지 여부

    Vec3 _velocity = Vec3(0.f, 0.f, 0.f);          // 선형 속도
    Vec3 _angularVelocity = Vec3(0.f, 0.f, 0.f);   // 각속도
};
