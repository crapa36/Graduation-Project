#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider {
public:
    SphereCollider();
    virtual ~SphereCollider();

    virtual void FinalUpdate() override;
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    BoundingSphere& GetBoundingSphere() { return _boundingSphere; }

    virtual void CreateMesh() override;
    virtual void Render() override;

    void SetRadius(float radius) { _radius = radius; }
    void SetCenter(Vec3 center) { _center = center; }

private:

    float		_radius = 1.f;
    Vec3		_center = Vec3(0, 0, 0);

    shared_ptr<class Mesh> _mesh;
    shared_ptr<class Material> _material;

    BoundingSphere _boundingSphere;
};
