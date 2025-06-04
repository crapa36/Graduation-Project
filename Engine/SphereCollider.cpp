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

// SphereCollider 생성자 및 소멸자
SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere) {
}

SphereCollider::~SphereCollider() {
}

// 최종 업데이트: 스피어 콜라이더의 중심과 반경 업데이트
void SphereCollider::FinalUpdate() {
    _boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition() + _center;
    _boundingSphere.Radius = _radius / 2;
}

// 레이와의 교차 검사
bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {
    return _boundingSphere.Intersects(rayOrigin, rayDir, distance);
}

// 다른 콜라이더와의 교차 검사
bool SphereCollider::Intersects(const shared_ptr<BaseCollider>& other) {
    if (!other) return false;

    ColliderType type = other->GetColliderType();

    switch (type) {
    case ColliderType::Sphere:
    {
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        if (!otherSphere) return false; // 안전성 강화
        return _boundingSphere.Intersects(otherSphere->GetBoundingSphere());
    }
    case ColliderType::Box:
    {
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        if (!otherBox) return false; // 안전성 강화
        return _boundingSphere.Intersects(otherBox->GetBoundingBox());
    }
    default:
        return false;
    }
}

// 레이과의 충돌 법선 계산
Vec3 SphereCollider::GetCollisionNormal(const Vec4& rayOrigin, const Vec4& rayDir) {
    Vec3 normal(0, 0, 0);
    float distance;

    if (!Intersects(rayOrigin, rayDir, distance)) {
        return normal; // 충돌이 없으면 빈 벡터 반환
    }

    Vec4 hitPoint = rayOrigin + rayDir * distance;
    Vec3 sphereCenter = _boundingSphere.Center;
    Vec3 hitPointVec(hitPoint.x, hitPoint.y, hitPoint.z);
    Vec3 normalVec = hitPointVec - sphereCenter;
    normalVec.Normalize();
    normal = normalVec;

    return normal;
}

// 다른 콜라이더와의 충돌 법선 계산
Vec3 SphereCollider::GetCollisionNormal(const shared_ptr<BaseCollider>& other) {
    Vec3 normal;

    if (!other) return normal;

    ColliderType otherType = other->GetColliderType();
    if (otherType == ColliderType::Sphere) {

        // Sphere-Sphere 충돌
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        if (!otherSphere) {

            // 에러 처리: 동적 캐스트 실패
            return normal;
        }
        Vec3 otherCenter = otherSphere->GetBoundingSphere().Center;
        Vec3 thisCenter = _boundingSphere.Center;
        Vec3 normalVec = otherCenter - thisCenter;
        normalVec.Normalize();
        normal = normalVec;
    }
    else if (otherType == ColliderType::Box) {

        // Sphere-Box 충돌
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        if (!otherBox) {

            // 에러 처리: 동적 캐스트 실패
            return normal;
        }
        auto otherBoundingBox = otherBox->GetBoundingBox();
        Vec3 boxCenter = otherBoundingBox.Center;
        Vec3 boxExtents = otherBoundingBox.Extents;
        Matrix boxRotation = otherBox->GetRotationMatrix();

        // 박스 로컬 좌표계로 스피어 중심 변환
        Vec3 sphereCenter = _boundingSphere.Center;
        Matrix invBoxRotation = boxRotation.Invert();
        Vec3 localSphereCenter = Vector3::Transform(sphereCenter - boxCenter, invBoxRotation);

        // 박스의 가장 가까운 점을 계산
        Vec3 closestPoint;
        closestPoint.x = std::clamp(localSphereCenter.x, -boxExtents.x, boxExtents.x);
        closestPoint.y = std::clamp(localSphereCenter.y, -boxExtents.y, boxExtents.y);
        closestPoint.z = std::clamp(localSphereCenter.z, -boxExtents.z, boxExtents.z);

        // 로컬 좌표계에서 법선 벡터 계산
        Vec3 localNormal = localSphereCenter - closestPoint;

        // 법선 벡터가 0인 경우 처리 (박스 표면에 위치)
        if (localNormal.Length() < FLT_EPSILON) {
            if (abs(closestPoint.x - boxExtents.x) < FLT_EPSILON) localNormal = Vec3(1, 0, 0);
            else if (abs(closestPoint.x + boxExtents.x) < FLT_EPSILON) localNormal = Vec3(-1, 0, 0);
            else if (abs(closestPoint.y - boxExtents.y) < FLT_EPSILON) localNormal = Vec3(0, 1, 0);
            else if (abs(closestPoint.y + boxExtents.y) < FLT_EPSILON) localNormal = Vec3(0, -1, 0);
            else if (abs(closestPoint.z - boxExtents.z) < FLT_EPSILON) localNormal = Vec3(0, 0, 1);
            else if (abs(closestPoint.z + boxExtents.z) < FLT_EPSILON) localNormal = Vec3(0, 0, -1);
        }
        else {
            localNormal.Normalize();
        }

        // 월드 좌표계로 변환 (법선 벡터는 방향만 변환)
        normal = -Vector3::TransformNormal(localNormal, boxRotation);
    }

    return normal;
}

// 다른 콜라이더와의 충돌 깊이 계산
float SphereCollider::GetCollisionDepth(const shared_ptr<BaseCollider>& other) {
    float depth = 0.0f;
    if (!other) return depth;

    ColliderType otherType = other->GetColliderType();

    if (otherType == ColliderType::Sphere) {

        // Sphere-Sphere 충돌 깊이
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        if (!otherSphere) {

            // 에러 처리: 동적 캐스트 실패
            return depth;
        }
        Vec3 otherCenter = otherSphere->GetBoundingSphere().Center;
        Vec3 thisCenter = _boundingSphere.Center;
        float distance = (thisCenter - otherCenter).Length();
        depth = (_boundingSphere.Radius + otherSphere->GetBoundingSphere().Radius) - distance;
    }
    else if (otherType == ColliderType::Box) {

        // Sphere-Box 충돌 깊이
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        if (!otherBox) {

            // 에러 처리: 동적 캐스트 실패
            return depth;
        }
        auto otherBoundingBox = otherBox->GetBoundingBox();
        Vec3 boxCenter = otherBoundingBox.Center;
        Vec3 boxExtents = otherBoundingBox.Extents;
        Matrix boxRotation = otherBox->GetRotationMatrix();

    _mesh = GResources->LoadSphereMesh();
    _material = GResources->Get<Material>(L"Collider")->Clone();
#endif
        Vec3 localSphereCenter = Vector3::Transform(sphereCenter - boxCenter, invBoxRotation);

        // 박스의 가장 가까운 점 찾기
        Vec3 closestPoint;
        closestPoint.x = std::clamp(localSphereCenter.x, -boxExtents.x, boxExtents.x);
        closestPoint.y = std::clamp(localSphereCenter.y, -boxExtents.y, boxExtents.y);
        closestPoint.z = std::clamp(localSphereCenter.z, -boxExtents.z, boxExtents.z);

        // 가장 가까운 점과 스피어 중심 간 거리 계산
        float distance = (closestPoint - localSphereCenter).Length();
        depth = _boundingSphere.Radius - distance;
    }
    if (depth < 0.0f)
        depth = 0.0f;
    return depth;
}

#ifdef _DEBUG

// 디버그용 메쉬 생성
void SphereCollider::CreateMesh() {
    _mesh = GET_SINGLETON(Resources)->LoadSphereMesh();
    _material = GET_SINGLETON(Resources)->Get<Material>(L"Collider")->Clone();

    _DebugObject = make_shared<GameObject>();
    _DebugObject->AddComponent(make_shared<Transform>());

    shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

    meshRenderer->SetMesh(_mesh);
    meshRenderer->SetMaterial(_material);

    _DebugObject->AddComponent(meshRenderer);
}

// 디버그용 렌더링
void SphereCollider::Render() {
    if (_DebugObject == nullptr)
        CreateMesh();
    _DebugObject->GetTransform()->SetLocalPosition(_boundingSphere.Center);
    Vec3 scale = _radius * Vec3(1, 1, 1);
    _DebugObject->GetTransform()->SetLocalScale(scale);
    _DebugObject->GetTransform()->FinalUpdate();
    _DebugObject->GetMeshRenderer()->Render();
}
#endif