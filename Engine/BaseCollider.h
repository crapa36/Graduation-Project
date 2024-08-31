#pragma once
#include "Component.h"

enum class ColliderType {
    Sphere,
    Box
};

class BaseCollider : public Component {
public:
    BaseCollider(ColliderType colliderType);
    virtual ~BaseCollider();

    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) = 0;

    virtual Vec3 GetCollisionNormal(const shared_ptr<BaseCollider>& other) = 0;
    virtual float GetCollisionDepth(const shared_ptr<BaseCollider>& other) = 0;

    virtual float GetHeight() = 0;

    ColliderType GetColliderType() { return _colliderType; }

public:
    virtual void SetCenter(Vec3 center) = 0;

    virtual void SetRadius(float radius) = 0;

    virtual void SetExtents(Vec3 extents) = 0;
    virtual void SetOrientation(const XMFLOAT4& orientation) = 0;

#ifdef _DEBUG
    virtual void CreateMesh() = 0;
    virtual void Render() = 0;
#endif

private:
    ColliderType _colliderType = {};
};