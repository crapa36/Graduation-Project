#pragma once
#include "Component.h"

enum class ColliderType {
    Sphere,
    AABB,
    OBB,
};

class BaseCollider : public Component {
public:
    BaseCollider(ColliderType colliderType);
    virtual ~BaseCollider();

    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;

    virtual bool Intersects(const shared_ptr<BaseCollider>& other) = 0;
    ColliderType GetColliderType() { return _colliderType; }

#ifdef _DEBUG
    virtual void CreateMesh() = 0;
    virtual void Render() = 0;
#endif

private:
    ColliderType _colliderType = {};
};