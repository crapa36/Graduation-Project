#include "pch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include <algorithm>

BoxCollider::BoxCollider() : BaseCollider(ColliderType::Box) {
}

BoxCollider::~BoxCollider() {}

void BoxCollider::FinalUpdate() {
    _boundingBox.Center = GetGameObject()->GetTransform()->GetWorldPosition() + _center;
    _boundingBox.Extents = _extents / 2;
    _boundingBox.Orientation = _orientation;
}

bool BoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {
    return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

bool BoxCollider::Intersects(const shared_ptr<BaseCollider>& other) {
    if (!other) return false;

    switch (other->GetColliderType()) {
    case ColliderType::Sphere:
        return _boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
    case ColliderType::Box:
        return _boundingBox.Intersects(dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox());
    default:
        return false;
    }
}

Vec3 BoxCollider::GetCollisionNormal(const Vec4& rayOrigin, const Vec4& rayDir) {
    Vec3 normal(0, 0, 0);
    float distance;

    if (!Intersects(rayOrigin, rayDir, OUT distance)) {
        return normal; // 충돌이 없으면 빈 벡터 반환
    }

    Vec4 hitPoint = rayOrigin + rayDir * distance;
    Vec3 localHitPoint = XMVector3TransformCoord(XMLoadFloat4(&hitPoint), XMMatrixInverse(nullptr, GetRotationMatrix()));

    // 박스의 가장 가까운 면을 찾기
    const float epsilon = FLT_EPSILON;
    if (abs(localHitPoint.x - _boundingBox.Extents.x) < epsilon) normal = Vec3(1, 0, 0);
    else if (abs(localHitPoint.x + _boundingBox.Extents.x) < epsilon) normal = Vec3(-1, 0, 0);
    else if (abs(localHitPoint.y - _boundingBox.Extents.y) < epsilon) normal = Vec3(0, 1, 0);
    else if (abs(localHitPoint.y + _boundingBox.Extents.y) < epsilon) normal = Vec3(0, -1, 0);
    else if (abs(localHitPoint.z - _boundingBox.Extents.z) < epsilon) normal = Vec3(0, 0, 1);
    else if (abs(localHitPoint.z + _boundingBox.Extents.z) < epsilon) normal = Vec3(0, 0, -1);

    // 월드 좌표계로 변환
    normal = XMVector3TransformNormal(XMLoadFloat3(&normal), GetRotationMatrix());
    return normal;
}

Vec3 BoxCollider::GetCollisionNormal(const shared_ptr<BaseCollider>& other) {
    Vec3 normal;

    ColliderType otherType = other->GetColliderType();
    if (otherType == ColliderType::Sphere) {
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        Vec3 sphereCenter = otherSphere->GetBoundingSphere().Center;
        Vec3 boxCenter = _boundingBox.Center;
        Vec3 boxExtents = _boundingBox.Extents;
        XMMATRIX boxRotation = GetRotationMatrix();

        // 박스 로컬 좌표계로 스피어 중심 변환
        XMVECTOR sphereCenterVec = XMLoadFloat3(&sphereCenter);
        XMVECTOR boxCenterVec = XMLoadFloat3(&boxCenter);
        XMVECTOR localSphereCenter = XMVector3Transform(sphereCenterVec - boxCenterVec, XMMatrixInverse(nullptr, boxRotation));

        // 박스의 가장 가까운 점 계산
        XMFLOAT3 localSphereCenterFloat;
        XMStoreFloat3(&localSphereCenterFloat, localSphereCenter);
        XMFLOAT3 closestPoint;
        closestPoint.x = std::clamp(localSphereCenterFloat.x, -boxExtents.x, boxExtents.x);
        closestPoint.y = std::clamp(localSphereCenterFloat.y, -boxExtents.y, boxExtents.y);
        closestPoint.z = std::clamp(localSphereCenterFloat.z, -boxExtents.z, boxExtents.z);

        // 로컬 좌표계에서 법선 벡터 계산
        XMVECTOR closestPointVec = XMLoadFloat3(&closestPoint);
        XMVECTOR localNormal = localSphereCenter - closestPointVec;

        // 법선 벡터가 0인 경우 처리
        if (XMVector3Length(localNormal).m128_f32[0] < FLT_EPSILON) {
            if (abs(closestPoint.x - boxExtents.x) < FLT_EPSILON) localNormal = XMVectorSet(1, 0, 0, 0);
            else if (abs(closestPoint.x + boxExtents.x) < FLT_EPSILON) localNormal = XMVectorSet(-1, 0, 0, 0);
            else if (abs(closestPoint.y - boxExtents.y) < FLT_EPSILON) localNormal = XMVectorSet(0, 1, 0, 0);
            else if (abs(closestPoint.y + boxExtents.y) < FLT_EPSILON) localNormal = XMVectorSet(0, -1, 0, 0);
            else if (abs(closestPoint.z - boxExtents.z) < FLT_EPSILON) localNormal = XMVectorSet(0, 0, 1, 0);
            else if (abs(closestPoint.z + boxExtents.z) < FLT_EPSILON) localNormal = XMVectorSet(0, 0, -1, 0);
        }
        else {
            localNormal = XMVector3Normalize(localNormal);
        }

        // 월드 좌표계로 변환
        XMVECTOR worldNormal = XMVector3TransformNormal(localNormal, boxRotation);
        XMStoreFloat3(&normal, worldNormal);
    }
    else if (otherType == ColliderType::Box) {
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        Vec3 otherCenter = otherBox->GetBoundingBox().Center;
        Vec3 thisCenter = _boundingBox.Center;
        Vec3 delta = thisCenter - otherCenter;

        // 가장 큰 축을 따라 법선 결정
        if (abs(delta.x) > abs(delta.y) && abs(delta.x) > abs(delta.z)) {
            normal = -Vec3((delta.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f);
        }
        else if (abs(delta.y) > abs(delta.x) && abs(delta.y) > abs(delta.z)) {
            normal = -Vec3(0.0f, (delta.y > 0) ? 1.0f : -1.0f, 0.0f);
        }
        else {
            normal = -Vec3(0.0f, 0.0f, (delta.z > 0) ? 1.0f : -1.0f);
        }
    }

    return normal;
}

float BoxCollider::GetCollisionDepth(const shared_ptr<BaseCollider>& other) {
    float depth = 0.0f;

    ColliderType otherType = other->GetColliderType();
    if (otherType == ColliderType::Sphere) {
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        Vec3 sphereCenter = otherSphere->GetBoundingSphere().Center;
        Vec3 boxCenter = _boundingBox.Center;
        Vec3 boxExtents = _boundingBox.Extents;
        XMMATRIX boxRotation = GetRotationMatrix();

        // 박스 로컬 좌표계로 스피어 중심 변환
        XMVECTOR sphereCenterVec = XMLoadFloat3(&sphereCenter);
        XMVECTOR boxCenterVec = XMLoadFloat3(&boxCenter);
        XMVECTOR localSphereCenter = XMVector3Transform(sphereCenterVec - boxCenterVec, XMMatrixInverse(nullptr, boxRotation));

        // 박스의 가장 가까운 점 찾기
        XMFLOAT3 localSphereCenterFloat;
        XMStoreFloat3(&localSphereCenterFloat, localSphereCenter);
        XMFLOAT3 closestPoint;
        closestPoint.x = std::clamp(localSphereCenterFloat.x, -boxExtents.x, boxExtents.x);
        closestPoint.y = std::clamp(localSphereCenterFloat.y, -boxExtents.y, boxExtents.y);
        closestPoint.z = std::clamp(localSphereCenterFloat.z, -boxExtents.z, boxExtents.z);

        // 가장 가까운 점과 스피어 중심 간 거리 계산
        XMVECTOR closestPointVec = XMLoadFloat3(&closestPoint);
        float distance = XMVector3Length(closestPointVec - localSphereCenter).m128_f32[0];
        depth = otherSphere->GetBoundingSphere().Radius - distance;
    }
    else if (otherType == ColliderType::Box) {
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        Vec3 otherCenter = otherBox->GetBoundingBox().Center;
        Vec3 thisCenter = _boundingBox.Center;
        Vec3 thisExtents = _boundingBox.Extents;
        Vec3 otherExtents = otherBox->GetBoundingBox().Extents;

        Vec3 overlap;
        overlap.x = (thisExtents.x + otherExtents.x) - abs(thisCenter.x - otherCenter.x);
        overlap.y = (thisExtents.y + otherExtents.y) - abs(thisCenter.y - otherCenter.y);
        overlap.z = (thisExtents.z + otherExtents.z) - abs(thisCenter.z - otherCenter.z);

        // 가장 작은 축을 따라 충돌 깊이 결정
        depth = min(overlap.x, min(overlap.y, overlap.z));
    }

    return depth < 0.0f ? 0.0f : depth;
}

#ifdef _DEBUG
void BoxCollider::CreateMesh() {
    _mesh = GET_SINGLETON(Resources)->LoadCubeMesh();
    _material = GET_SINGLETON(Resources)->Get<Material>(L"Collider")->Clone();
    _DebugObject = make_shared<GameObject>();
    _DebugObject->AddComponent(make_shared<Transform>());

    auto meshRenderer = make_shared<MeshRenderer>();
    meshRenderer->SetMesh(_mesh);
    meshRenderer->SetMaterial(_material);
    _DebugObject->AddComponent(meshRenderer);
}

void BoxCollider::Render() {
    if (!_DebugObject) CreateMesh();

    _DebugObject->GetTransform()->SetLocalPosition(_boundingBox.Center);
    _DebugObject->GetTransform()->SetLocalScale(_extents);
    _DebugObject->GetTransform()->FinalUpdate();
    _DebugObject->GetMeshRenderer()->Render();
}
#endif