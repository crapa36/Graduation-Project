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
    Vec3 normal(0.0f, 0.0f, 0.0f);
    float distance;

    // 충돌 여부 확인
    if (!Intersects(rayOrigin, rayDir, distance)) {
        return normal; // 충돌이 없으면 빈 벡터 반환
    }

    // 충돌 지점 계산
    Vec4 hitPoint = rayOrigin + rayDir * distance;

    // 박스의 회전 행렬의 역행렬 계산
    Matrix invRotationMatrix = GetRotationMatrix();
    invRotationMatrix = invRotationMatrix.Invert();

    // 충돌 지점을 로컬 좌표계로 변환
    Vec3 hitPointVec(hitPoint.x, hitPoint.y, hitPoint.z);
    Vec3 localHitPoint = Vector3::Transform(hitPointVec, invRotationMatrix);

    // 박스의 가장 가까운 면을 찾기 위한 절대값 비교
    Vec3 absLocalHitPoint = Vec3(
        std::abs(localHitPoint.x),
        std::abs(localHitPoint.y),
        std::abs(localHitPoint.z)
    );

    // 가장 큰 절대값을 가지는 축을 찾아 해당 축의 노멀을 설정
    if (absLocalHitPoint.x > absLocalHitPoint.y && absLocalHitPoint.x > absLocalHitPoint.z) {
        normal = (localHitPoint.x > 0) ? Vec3(1.0f, 0.0f, 0.0f) : Vec3(-1.0f, 0.0f, 0.0f);
    }
    else if (absLocalHitPoint.y > absLocalHitPoint.x && absLocalHitPoint.y > absLocalHitPoint.z) {
        normal = (localHitPoint.y > 0) ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(0.0f, -1.0f, 0.0f);
    }
    else {
        normal = (localHitPoint.z > 0) ? Vec3(0.0f, 0.0f, 1.0f) : Vec3(0.0f, 0.0f, -1.0f);
    }

    // 노멀을 월드 좌표계로 변환
    normal = Vector3::TransformNormal(normal, GetRotationMatrix());

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

//#include "DebugLineManager.h"
//
//void BoxCollider::Render() {
//    Vec3 corners[8];
//
//    // 박스의 8개 꼭지점 계산
//    Vec3 extents = _boundingBox.Extents;
//    XMMATRIX rotationMatrix = GetRotationMatrix();
//    Vec3 center = _boundingBox.Center;
//
//    // 박스의 로컬 좌표에서 각 꼭지점 계산
//    Vec3 localCorners[8] = {
//        Vec3(-extents.x, -extents.y, -extents.z),
//        Vec3(extents.x, -extents.y, -extents.z),
//        Vec3(extents.x,  extents.y, -extents.z),
//        Vec3(-extents.x,  extents.y, -extents.z),
//        Vec3(-extents.x, -extents.y,  extents.z),
//        Vec3(extents.x, -extents.y,  extents.z),
//        Vec3(extents.x,  extents.y,  extents.z),
//        Vec3(-extents.x,  extents.y,  extents.z)
//    };
//
//    // 월드 좌표로 변환
//    for (int i = 0; i < 8; ++i) {
//        XMVECTOR cornerVec = XMVector3Transform(XMLoadFloat3(&localCorners[i]), rotationMatrix);
//        cornerVec += XMLoadFloat3(&center);
//        XMStoreFloat3(&corners[i], cornerVec);
//    }
//
//    // 각 변을 디버그 라인으로 추가
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[0], corners[1]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[1], corners[2]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[2], corners[3]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[3], corners[0]);
//
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[4], corners[5]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[5], corners[6]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[6], corners[7]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[7], corners[4]);
//
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[0], corners[4]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[1], corners[5]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[2], corners[6]);
//    GET_SINGLETON(DebugLineManager)->AddLine(corners[3], corners[7]);
//}
#endif