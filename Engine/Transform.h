#pragma once
#include "Component.h"

class Transform : public Component {
public:
    Transform();
    virtual ~Transform();

    virtual void FinalUpdate() override;
    void PushData();

public:

    // Parent 기준
    const Vec3& GetLocalPosition() { return _localPosition; }
    const Vec3& GetLocalRotation() { return _localRotation; }
    const Vec3& GetLocalScale() { return _localScale; }

    // TEMP
    float GetBoundingSphereRadius() { return max(max(_localScale.x, _localScale.y), _localScale.z); }

    const Matrix& GetLocalToWorldMatrix() { return _matWorld; }
    Vec3 GetWorldPosition() { return _matWorld.Translation(); }

    Vec3 GetRight() { return _matWorld.Right(); }
    Vec3 GetUp() { return _matWorld.Up(); }
    Vec3 GetLook() { return _matWorld.Backward(); }

    void SetLocalPosition(const Vec3& position) { _localPosition = position; }
    void SetLocalRotation(const Vec3& rotation) { _localRotation = rotation; }
    void SetLocalScale(const Vec3& scale) { _localScale = scale; }

    void SetInheritPosition(bool inherit) { _inheritPosition = inherit; }
    void SetInheritRotation(bool inherit) { _inheritRotation = inherit; }
    void SetInheritScale(bool inherit) { _inheritScale = inherit; }

    void LookAt(const Vec3& dir);
    void SetLocalRotationQuaternion(const DirectX::SimpleMath::Quaternion& quaternion);
    static bool CloseEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());
    static Vec3 DecomposeRotationMatrix(const Matrix& rotation);

    void SetAcceleration(Vec3 acceleration) { _acceleration = acceleration; }
    Vec3 GetAcceleration() { return _acceleration; }

    void SetVelocity(Vec3 velocity) { _velocity = velocity; }
    Vec3 GetVelocity() { return _velocity; }

public:
    void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
    weak_ptr<Transform> GetParent() { return _parent; }

private:

    bool _inheritPosition = true;
    bool _inheritRotation = true;
    bool _inheritScale = true;

    // Parent 기준
    Vec3 _localPosition = {};
    Vec3 _localRotation = {};
    Vec3 _localScale = { 1.f, 1.f, 1.f };

    Matrix _matLocal = {};
    Matrix _matWorld = {};

    weak_ptr<Transform> _parent;

    Vec3 _acceleration;
    Vec3 _velocity;
};
