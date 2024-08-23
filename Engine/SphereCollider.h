#pragma once
#include "BaseCollider.h"

class SphereCollider : public BaseCollider {
public:
    SphereCollider();
    virtual ~SphereCollider();

    virtual void FinalUpdate() override;
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    virtual Vec4 GetCollisionNormal(const shared_ptr<BaseCollider>& other) override;
    virtual float GetCollisionDepth(const shared_ptr<BaseCollider>& other) override;

    BoundingSphere& GetBoundingSphere() { return _boundingSphere; }

    virtual float GetHeight() override { return _radius / 2; }

    virtual void CreateMesh() override;
    virtual void Render() override;

    void SetRadius(float radius) override { _radius = radius; }
    void SetCenter(Vec3 center) override { _center = center; }

    void SetExtents(Vec3 extents) override {

        // Sphere does not use extents, but must implement the method
    }
    void SetOrientation(const XMFLOAT4& orientation) override {

        // Sphere does not use orientation, but must implement the method
    }
private:

    float		_radius = 1.f;
    Vec3		_center = Vec3(0, 0, 0);

    BoundingSphere _boundingSphere;

#ifdef _DEBUG
    shared_ptr<class Mesh> _mesh;
    shared_ptr<class Material> _material;
    shared_ptr<GameObject> _DebugObject;
#endif // DEBUG
};
