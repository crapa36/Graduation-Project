#pragma once
#include "BaseCollider.h"
#include "DirectXCollision.h"

class BoxCollider : public BaseCollider {
public:
    BoxCollider();
    virtual ~BoxCollider();

    virtual void FinalUpdate() override;
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    virtual Vec4 GetCollisionNormal(const shared_ptr<BaseCollider>& other) override;
    virtual float GetCollisionDepth(const shared_ptr<BaseCollider>& other) override;

    BoundingOrientedBox GetBoundingBox() const { return _boundingBox; }

    virtual void CreateMesh() override;
    virtual void Render() override;

    void SetCenter(Vec3 center) override { _center = center; }
    void SetExtents(Vec3 extents) override { _extents = extents; }
    void SetOrientation(const XMFLOAT4& orientation) override { _orientation = orientation; }

    void SetRadius(float radius) override {

        // Box does not use radius, but must implement the method
    }

    Vec3 GetCenter() const { return _center; }
    Vec3 GetExtents() const { return _extents; }
    XMFLOAT4 GetOrientation() const { return _orientation; }
    DirectX::XMMATRIX GetRotationMatrix() const {
        return DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&_orientation));
    }
private:

    Vec3 _extents = Vec3(0, 0, 0);
    Vec3 _center = Vec3(0, 0, 0);
    XMFLOAT4 _orientation = XMFLOAT4(0, 0, 0, 1.f);

    shared_ptr<class Mesh> _mesh;
    shared_ptr<class Material> _material;

    shared_ptr<class GameObject> _DebugObject;

    BoundingOrientedBox _boundingBox;
};