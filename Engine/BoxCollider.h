#pragma once
#include "BaseCollider.h"

class BoxCollider : public BaseCollider {
public:
    BoxCollider();
    virtual ~BoxCollider();

    virtual void FinalUpdate() override;
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    BoundingBox GetBoundingBox() const { return _boundingBox; }

    virtual void CreateMesh() override;
    virtual void Render() override;

    void SetCenter(Vec3 center) { _center = center; }
    void SetExtents(Vec3 extents) { _extents = extents; }

    Vec3 GetCenter() const { return _center; }
    Vec3 GetExtents() const { return _extents; }
private:

    Vec3		_extents = Vec3(0, 0, 0);
    Vec3		_center = Vec3(0, 0, 0);

    shared_ptr<class Mesh> _mesh;
    shared_ptr<class Material> _material;

    BoundingBox _boundingBox;
};