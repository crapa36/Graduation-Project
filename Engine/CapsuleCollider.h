#pragma once
#include "BaseCollider.h"
#include "DirectXCollision.h"

class CapsuleCollider : public BaseCollider {
public:
    CapsuleCollider();
    virtual ~CapsuleCollider();

    virtual void FinalUpdate() override;
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    virtual Vec3 GetCollisionNormal(const Vec4& rayOrigin, const Vec4& rayDir) override;
    virtual Vec3 GetCollisionNormal(const shared_ptr<BaseCollider>& other) override;
    virtual float GetCollisionDepth(const shared_ptr<BaseCollider>& other) override;

    struct BoundingCapsule {
        Vec3 Center;
        Vec3 Axis;
        float Radius;
        float Height;
    };

    BoundingCapsule GetBoundingCapsule() const { return _boundingCapsule; }

    virtual float GetHeight() override { return _height; }
    virtual Vec3 GetCenter() override { return _center; }
    virtual float GetRadius() override { return _radius; }
    virtual Vec3 GetExtents() override { return Vec3(_radius, _height / 2, _radius); }

#ifdef _DEBUG
    virtual void CreateMesh() override;
    virtual void Render() override;
#endif

    void SetCenter(Vec3 center) override { _center = center; }
    void SetRadius(float radius) override { _radius = radius; }
    void SetHeight(float height) { _height = height; }

    Vec3 GetCenter() const { return _center; }
    float GetRadius() const { return _radius; }
    float GetHeight() const { return _height; }
    Vec4 GetOrientation() const { return _orientation; }
    Matrix GetRotationMatrix() const {
        return Matrix::CreateFromQuaternion(Quaternion(_orientation));
    }

private:
    float _radius = 0.5f;
    float _height = 2.0f;
    Vec3 _center = Vec3(0, 0, 0);
    Vec4 _orientation = Vec4(0, 0, 0, 1);

    shared_ptr<class Mesh> _mesh;
    shared_ptr<class Material> _material;

    shared_ptr<class GameObject> _DebugObject;

    BoundingCapsule _boundingCapsule;

    // Helper functions
    Vec3 ClosestPointOnLineSegment(const Vec3& A, const Vec3& B, const Vec3& Point);
    bool IntersectsCapsuleSphere(const BoundingSphere& sphere);
    bool IntersectsCapsuleBox(const BoundingOrientedBox& box);
    bool IntersectsCapsuleCapsule(const BoundingCapsule& other);
    Vec3 ClosestPointOnLineSegmentToBox(const Vec3& lineStart, const Vec3& lineEnd, const BoundingOrientedBox& box);
    void ClosestPointsBetweenLines(const Vec3& line1Start, const Vec3& line1End, const Vec3& line2Start, const Vec3& line2End, Vec3& closestPoint1, Vec3& closestPoint2);
};