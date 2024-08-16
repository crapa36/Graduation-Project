#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider {
public:
    SphereCollider();
    virtual ~SphereCollider();

    virtual void FinalUpdate() override;
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    // 이 구체 충돌 감지기의 경계 구체를 반환
    BoundingSphere& GetBoundingSphere() { return _boundingSphere; }

    void SetRadius(float radius) { _radius = radius; }
    void SetCenter(Vec3 center) { _center = center; }

private:

    // Local 기준
    float		_radius = 1.f;
    Vec3		_center = Vec3(0, 0, 0);

    BoundingSphere _boundingSphere;
};
