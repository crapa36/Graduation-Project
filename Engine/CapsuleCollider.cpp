#include "pch.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include <algorithm>

CapsuleCollider::CapsuleCollider() : BaseCollider(ColliderType::Capsule) {
}

CapsuleCollider::~CapsuleCollider() {}

void CapsuleCollider::FinalUpdate() {
    _boundingCapsule.Center = GetGameObject()->GetTransform()->GetWorldPosition() + _center;
    _boundingCapsule.Radius = _radius;
    _boundingCapsule.Height = _height;
    _boundingCapsule.Axis = Vec3::Transform(Vec3::Up, Matrix::CreateFromQuaternion(Quaternion(_orientation)));
}

bool CapsuleCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {
    Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
    Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);

    Vec3 closestPoint = ClosestPointOnLineSegment(capsuleStart, capsuleEnd, Vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z));
    Vec3 rayToClosest = closestPoint - Vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);

    float a = rayDir.Dot(rayDir);
    float b = 2.0f * rayToClosest.Dot(Vec3(rayDir.x, rayDir.y, rayDir.z));
    float c = rayToClosest.Dot(rayToClosest) - _boundingCapsule.Radius * _boundingCapsule.Radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return false;

    distance = (-b - sqrt(discriminant)) / (2 * a);
    return distance >= 0;
}

bool CapsuleCollider::Intersects(const shared_ptr<BaseCollider>& other) {
    if (!other) return false;

    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return IntersectsCapsuleSphere(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
    case ColliderType::Box:
        return IntersectsCapsuleBox(dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox());
    case ColliderType::Capsule:
        return IntersectsCapsuleCapsule(dynamic_pointer_cast<CapsuleCollider>(other)->GetBoundingCapsule());
    default:
        return false;
    }
}

Vec3 CapsuleCollider::GetCollisionNormal(const Vec4& rayOrigin, const Vec4& rayDir) {
    float distance;
    if (!Intersects(rayOrigin, rayDir, distance))
        return Vec3::Zero;

    Vec3 hitPoint = Vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z) + Vec3(rayDir.x, rayDir.y, rayDir.z) * distance;
    Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
    Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);

    Vec3 closestPoint = ClosestPointOnLineSegment(capsuleStart, capsuleEnd, hitPoint);
    auto collisionNormal = (hitPoint - closestPoint);
    collisionNormal.Normalize();
    return collisionNormal;
}

Vec3 CapsuleCollider::GetCollisionNormal(const shared_ptr<BaseCollider>& other) {
    Vec3 normal;

    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
    {
        auto sphere = dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere();
        Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 closestPoint = ClosestPointOnLineSegment(capsuleStart, capsuleEnd, sphere.Center);
        normal = (sphere.Center - closestPoint);
        normal.Normalize();
        break;
    }
    case ColliderType::Box:
    {
        auto box = dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox();
        Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 closestPoint = ClosestPointOnLineSegmentToBox(capsuleStart, capsuleEnd, box);
        normal = (closestPoint - box.Center);
        normal.Normalize();
        break;
    }
    case ColliderType::Capsule:
    {
        auto otherCapsule = dynamic_pointer_cast<CapsuleCollider>(other)->GetBoundingCapsule();
        Vec3 capsule1Start = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsule1End = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsule2Start = otherCapsule.Center - otherCapsule.Axis * (otherCapsule.Height * 0.5f - otherCapsule.Radius);
        Vec3 capsule2End = otherCapsule.Center + otherCapsule.Axis * (otherCapsule.Height * 0.5f - otherCapsule.Radius);

        Vec3 closestPoint1, closestPoint2;
        ClosestPointsBetweenLines(capsule1Start, capsule1End, capsule2Start, capsule2End, closestPoint1, closestPoint2);
        normal = (closestPoint2 - closestPoint1);
        normal.Normalize();
        break;
    }
    }

    return normal;
}

float CapsuleCollider::GetCollisionDepth(const shared_ptr<BaseCollider>& other) {
    float depth = 0.0f;

    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
    {
        auto sphere = dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere();
        Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 closestPoint = ClosestPointOnLineSegment(capsuleStart, capsuleEnd, sphere.Center);
        float distance = (sphere.Center - closestPoint).Length();
        depth = (_boundingCapsule.Radius + sphere.Radius) - distance;
        break;
    }
    case ColliderType::Box:
    {
        auto box = dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox();
        Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 closestPoint = ClosestPointOnLineSegmentToBox(capsuleStart, capsuleEnd, box);
        float distance = (closestPoint - box.Center).Length();
        depth = _boundingCapsule.Radius - distance;
        break;
    }
    case ColliderType::Capsule:
    {
        auto otherCapsule = dynamic_pointer_cast<CapsuleCollider>(other)->GetBoundingCapsule();
        Vec3 capsule1Start = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsule1End = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
        Vec3 capsule2Start = otherCapsule.Center - otherCapsule.Axis * (otherCapsule.Height * 0.5f - otherCapsule.Radius);
        Vec3 capsule2End = otherCapsule.Center + otherCapsule.Axis * (otherCapsule.Height * 0.5f - otherCapsule.Radius);

        Vec3 closestPoint1, closestPoint2;
        ClosestPointsBetweenLines(capsule1Start, capsule1End, capsule2Start, capsule2End, closestPoint1, closestPoint2);
        float distance = (closestPoint1 - closestPoint2).Length();
        depth = (_boundingCapsule.Radius + otherCapsule.Radius) - distance;
        break;
    }
    }

    return depth < 0.0f ? 0.0f : depth;
}

// Helper functions
Vec3 CapsuleCollider::ClosestPointOnLineSegment(const Vec3& A, const Vec3& B, const Vec3& Point) {
    Vec3 AB = B - A;
    float t = (Point - A).Dot(AB) / AB.Dot(AB);
    return A + AB * clamp(t, 0.0f, 1.0f);
}

bool CapsuleCollider::IntersectsCapsuleSphere(const BoundingSphere& sphere) {
    Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
    Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);

    Vec3 closestPoint = ClosestPointOnLineSegment(capsuleStart, capsuleEnd, sphere.Center);
    float distanceSquared = (sphere.Center - closestPoint).LengthSquared();
    float radiusSum = _boundingCapsule.Radius + sphere.Radius;

    return distanceSquared <= (radiusSum * radiusSum);
}

bool CapsuleCollider::IntersectsCapsuleBox(const BoundingOrientedBox& box) {

    // ������ �ٻ�ġ ���. ��Ȯ�� ����� ���ؼ��� �� ������ �˰����� �ʿ��մϴ�.
    Vec3 capsuleStart = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
    Vec3 capsuleEnd = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);

    Vec3 closestPoint = ClosestPointOnLineSegmentToBox(capsuleStart, capsuleEnd, box);
    float distanceSquared = (closestPoint - box.Center).LengthSquared();

    return distanceSquared <= (_boundingCapsule.Radius * _boundingCapsule.Radius);
}

bool CapsuleCollider::IntersectsCapsuleCapsule(const BoundingCapsule& other) {
    Vec3 capsule1Start = _boundingCapsule.Center - _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
    Vec3 capsule1End = _boundingCapsule.Center + _boundingCapsule.Axis * (_boundingCapsule.Height * 0.5f - _boundingCapsule.Radius);
    Vec3 capsule2Start = other.Center - other.Axis * (other.Height * 0.5f - other.Radius);
    Vec3 capsule2End = other.Center + other.Axis * (other.Height * 0.5f - other.Radius);

    Vec3 closestPoint1, closestPoint2;
    ClosestPointsBetweenLines(capsule1Start, capsule1End, capsule2Start, capsule2End, closestPoint1, closestPoint2);
    float distanceSquared = (closestPoint1 - closestPoint2).LengthSquared();
    float radiusSum = _boundingCapsule.Radius + other.Radius;

    return distanceSquared <= (radiusSum * radiusSum);
}

Vec3 CapsuleCollider::ClosestPointOnLineSegmentToBox(const Vec3& lineStart, const Vec3& lineEnd, const BoundingOrientedBox& box) {

    // �ڽ��� ���� ��ǥ��� ��ȯ
    Matrix invBoxRotation = Matrix::CreateFromQuaternion(Quaternion(box.Orientation)).Invert();
    Vec3 localLineStart = Vec3::Transform(lineStart - box.Center, invBoxRotation);
    Vec3 localLineEnd = Vec3::Transform(lineEnd - box.Center, invBoxRotation);

    // ������ ���� ����
    Vec3 lineDir = localLineEnd - localLineStart;

    // �ڽ��� extents
    float minX = -box.Extents.x;
    float maxX = box.Extents.x;
    float minY = -box.Extents.y;
    float maxY = box.Extents.y;
    float minZ = -box.Extents.z;
    float maxZ = box.Extents.z;

    // �� �ະ�� ���� Ŭ���Ʈ ����Ʈ ���
    float closestX, closestY, closestZ;

    // X�࿡ ���� Ŭ����
    if (lineDir.x != 0.0f) {
        float tMinX = (minX - localLineStart.x) / lineDir.x;
        float tMaxX = (maxX - localLineStart.x) / lineDir.x;
        float tClampMinX = max(0.0f, min(tMinX, tMaxX));
        float tClampMaxX = min(1.0f, max(tMinX, tMaxX));
        closestX = localLineStart.x + (localLineEnd.x - localLineStart.x) * clamp((tClampMinX + tClampMaxX) / 2.0f, 0.0f, 1.0f);
    }
    else {
        closestX = (localLineStart.x < minX) ? minX : ((localLineStart.x > maxX) ? maxX : localLineStart.x);
    }

    // Y�࿡ ���� Ŭ����
    if (lineDir.y != 0.0f) {
        float tMinY = (minY - localLineStart.y) / lineDir.y;
        float tMaxY = (maxY - localLineStart.y) / lineDir.y;
        float tClampMinY = max(0.0f, min(tMinY, tMaxY));
        float tClampMaxY = min(1.0f, max(tMinY, tMaxY));
        closestY = localLineStart.y + (localLineEnd.y - localLineStart.y) * clamp((tClampMinY + tClampMaxY) / 2.0f, 0.0f, 1.0f);
    }
    else {
        closestY = (localLineStart.y < minY) ? minY : ((localLineStart.y > maxY) ? maxY : localLineStart.y);
    }

    // Z�࿡ ���� Ŭ����
    if (lineDir.z != 0.0f) {
        float tMinZ = (minZ - localLineStart.z) / lineDir.z;
        float tMaxZ = (maxZ - localLineStart.z) / lineDir.z;
        float tClampMinZ = max(0.0f, min(tMinZ, tMaxZ));
        float tClampMaxZ = min(1.0f, max(tMinZ, tMaxZ));
        closestZ = localLineStart.z + (localLineEnd.z - localLineStart.z) * clamp((tClampMinZ + tClampMaxZ) / 2.0f, 0.0f, 1.0f);
    }
    else {
        closestZ = (localLineStart.z < minZ) ? minZ : ((localLineStart.z > maxZ) ? maxZ : localLineStart.z);
    }

    // ���� Ŭ���Ʈ ����Ʈ
    Vec3 closestPoint(closestX, closestY, closestZ);

    // ���� ��ǥ��� ��ȯ
    return Vec3::Transform(closestPoint, Matrix::CreateFromQuaternion(Quaternion(box.Orientation))) + box.Center;
}

void CapsuleCollider::ClosestPointsBetweenLines(const Vec3& line1Start, const Vec3& line1End, const Vec3& line2Start, const Vec3& line2End, Vec3& closestPoint1, Vec3& closestPoint2) {
    Vec3 u = line1End - line1Start;
    Vec3 v = line2End - line2Start;
    Vec3 w = line1Start - line2Start;

    float a = u.Dot(u);
    float b = u.Dot(v);
    float c = v.Dot(v);
    float d = u.Dot(w);
    float e = v.Dot(w);
    float D = a * c - b * b;

    float sc, tc;

    if (D < FLT_EPSILON) {
        sc = 0.0f;
        tc = (b > c ? d / b : e / c);
    }
    else {
        sc = (b * e - c * d) / D;
        tc = (a * e - b * d) / D;
    }

    sc = clamp(sc, 0.0f, 1.0f);
    tc = clamp(tc, 0.0f, 1.0f);

    closestPoint1 = line1Start + u * sc;
    closestPoint2 = line2Start + v * tc;
}

#ifdef _DEBUG
void CapsuleCollider::CreateMesh() {

    // ĸ�� �޽� ���� ����
    // �Ǹ����� �� ���� �ݱ��� �����Ͽ� ĸ�� ��� ����
    _mesh = GET_SINGLETON(Resources)->LoadSphereMesh();  // �ӽ÷� �� �޽� ���
    _material = GET_SINGLETON(Resources)->Get<Material>(L"Collider")->Clone();
    _DebugObject = make_shared<GameObject>();
    _DebugObject->AddComponent(make_shared<Transform>());

    auto meshRenderer = make_shared<MeshRenderer>();
    meshRenderer->SetMesh(_mesh);
    meshRenderer->SetMaterial(_material);
    _DebugObject->AddComponent(meshRenderer);
}

void CapsuleCollider::Render() {
    if (!_DebugObject) CreateMesh();

    _DebugObject->GetTransform()->SetLocalPosition(_boundingCapsule.Center);
    _DebugObject->GetTransform()->SetLocalScale(Vec3(_radius * 2, _height, _radius * 2));
    _DebugObject->GetTransform()->SetLocalRotation(Quaternion(_orientation));
    _DebugObject->GetTransform()->FinalUpdate();
    _DebugObject->GetMeshRenderer()->Render();
}
#endif