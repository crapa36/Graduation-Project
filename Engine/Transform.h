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
    const Vec3& GetLocalRevolution() { return _localRevolution; }
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
    void SetLocalRevolution(const Vec3& revolution) { _localRevolution = revolution; }
    void SetLocalScale(const Vec3& scale) { _localScale = scale; }

    void SetInheritPosition(bool inherit) { _inheritPosition = inherit; }
    void SetInheritRotation(bool inherit) { _inheritRotation = inherit; }
    void SetInheritRevolution(bool inherit) { _inheritRevolution = inherit; }
    void SetInheritScale(bool inherit) { _inheritScale = inherit; }

    void LookAt(const Vec3& dir);
    void SetLocalRotation(const Quaternion& quaternion) { _quaternion = quaternion; }
    static bool CloseEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());
    static Vec3 DecomposeRotationMatrix(const Matrix& rotation);
    Vec3 QuaternionToEuler(const Quaternion& q);

public:
    void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
    weak_ptr<Transform> GetParent() { return _parent; }

private:

    bool _inheritPosition = true;
    bool _inheritRotation = true;
    bool _inheritRevolution = true;
    bool _inheritScale = true;

    // Parent 기준
    Vec3 _localPosition = {};
    Vec3 _localRotation = {};
    Vec3 _localQuaternionRotation = {};
    Vec3 _localRevolution = {};
    Vec3 _localScale = { 1.f, 1.f, 1.f };
    Quaternion _quaternion;

    Matrix _matLocal = {};
    Matrix _matWorld = {};

    weak_ptr<Transform> _parent;
};
