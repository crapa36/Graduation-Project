#include "pch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere) {
    CreateMesh();
}

SphereCollider::~SphereCollider() {
}

void SphereCollider::FinalUpdate() {
    _boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition()+_center;

    _boundingSphere.Radius = _radius / 2;
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {
    return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(const shared_ptr<BaseCollider>& other) {
    ColliderType type = other->GetColliderType();

    switch (type) {
    case ColliderType::Sphere:
        return _boundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());

    case ColliderType::Box:
        return _boundingSphere.Intersects(dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox());
    }

    return false;
}

Vec3 SphereCollider::GetCollisionNormal(const shared_ptr<BaseCollider>& other) {
    Vec3 normal;

    ColliderType otherType = other->GetColliderType();
    if (otherType == ColliderType::Sphere) {

        // Sphere-Sphere 충돌
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        XMVECTOR otherCenter = XMLoadFloat3(&otherSphere->GetBoundingSphere().Center);
        XMVECTOR thisCenter = XMLoadFloat3(&_boundingSphere.Center);
        XMVECTOR normalVec = XMVectorSubtract(otherCenter, thisCenter);
        normalVec = XMVector3Normalize(normalVec);
        XMStoreFloat3(&normal, normalVec);
    }
    else if (otherType == ColliderType::Box) {

        // Sphere-Box 충돌
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        auto otherBoundingBox = otherBox->GetBoundingBox();
        XMVECTOR boxCenter = XMLoadFloat3(&otherBoundingBox.Center);
        XMVECTOR boxExtents = XMLoadFloat3(&otherBoundingBox.Extents);
        XMMATRIX boxRotation = otherBox->GetRotationMatrix();

        // 박스의 로컬 좌표계로 스피어 중심 변환
        XMVECTOR sphereCenter = XMLoadFloat3(&_boundingSphere.Center);
        XMMATRIX invBoxRotation = XMMatrixInverse(nullptr, boxRotation);
        XMVECTOR localSphereCenter = XMVector3Transform(sphereCenter - boxCenter, invBoxRotation);

        // 박스의 가장 가까운 점을 계산
        XMFLOAT3 localSphereCenterFloat;
        XMStoreFloat3(&localSphereCenterFloat, localSphereCenter);
        XMFLOAT3 closestPoint;
        closestPoint.x = std::clamp(localSphereCenterFloat.x, -XMVectorGetX(boxExtents), XMVectorGetX(boxExtents));
        closestPoint.y = std::clamp(localSphereCenterFloat.y, -XMVectorGetY(boxExtents), XMVectorGetY(boxExtents));
        closestPoint.z = std::clamp(localSphereCenterFloat.z, -XMVectorGetZ(boxExtents), XMVectorGetZ(boxExtents));

        // 로컬 좌표계에서 법선 벡터 계산
        XMVECTOR closestPointVec = XMLoadFloat3(&closestPoint);
        XMVECTOR localNormal = XMVectorSubtract(localSphereCenter, closestPointVec);

        // 법선 벡터가 0인 경우 처리 (박스 표면에 위치)
        if (XMVector3Length(localNormal).m128_f32[0] < FLT_EPSILON) {
            if (abs(closestPoint.x - XMVectorGetX(boxExtents)) < FLT_EPSILON) localNormal = XMVectorSet(1, 0, 0, 0);
            else if (abs(closestPoint.x + XMVectorGetX(boxExtents)) < FLT_EPSILON) localNormal = XMVectorSet(-1, 0, 0, 0);
            else if (abs(closestPoint.y - XMVectorGetY(boxExtents)) < FLT_EPSILON) localNormal = XMVectorSet(0, 1, 0, 0);
            else if (abs(closestPoint.y + XMVectorGetY(boxExtents)) < FLT_EPSILON) localNormal = XMVectorSet(0, -1, 0, 0);
            else if (abs(closestPoint.z - XMVectorGetZ(boxExtents)) < FLT_EPSILON) localNormal = XMVectorSet(0, 0, 1, 0);
            else if (abs(closestPoint.z + XMVectorGetZ(boxExtents)) < FLT_EPSILON) localNormal = XMVectorSet(0, 0, -1, 0);
        }
        else {
            localNormal = XMVector3Normalize(localNormal);
        }

        // 월드 좌표계로 변환
        XMVECTOR worldNormal = XMVector3TransformNormal(localNormal, boxRotation);
        XMStoreFloat3(&normal, worldNormal);
    }

    return normal;
}

float SphereCollider::GetCollisionDepth(const shared_ptr<BaseCollider>& other) {
    float depth = 0.0f;
    ColliderType otherType = other->GetColliderType();

    if (otherType == ColliderType::Sphere) {

        // Sphere-Sphere 충돌 깊이
        auto otherSphere = dynamic_pointer_cast<SphereCollider>(other);
        XMVECTOR otherCenter = XMLoadFloat3(&otherSphere->GetBoundingSphere().Center);
        XMVECTOR thisCenter = XMLoadFloat3(&_boundingSphere.Center);
        float distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(thisCenter, otherCenter)));
        depth = (_boundingSphere.Radius + otherSphere->GetBoundingSphere().Radius) - distance;
    }
    else if (otherType == ColliderType::Box) {

        // Sphere-Box 충돌 깊이
        auto otherBox = dynamic_pointer_cast<BoxCollider>(other);
        auto otherBoundingBox = otherBox->GetBoundingBox();
        XMVECTOR boxCenter = XMLoadFloat3(&otherBoundingBox.Center);
        XMVECTOR boxExtents = XMLoadFloat3(&otherBoundingBox.Extents);
        XMMATRIX boxRotation = otherBox->GetRotationMatrix();

        // 박스 로컬 좌표계로 스피어 중심 변환
        XMVECTOR sphereCenter = XMLoadFloat3(&_boundingSphere.Center);
        XMMATRIX invBoxRotation = XMMatrixInverse(nullptr, boxRotation);
        XMVECTOR localSphereCenter = XMVector3Transform(sphereCenter - boxCenter, invBoxRotation);

        // 박스의 가장 가까운 점 찾기
        XMFLOAT3 localSphereCenterFloat;
        XMStoreFloat3(&localSphereCenterFloat, localSphereCenter);
        XMFLOAT3 closestPoint;
        closestPoint.x = std::clamp(localSphereCenterFloat.x, -XMVectorGetX(boxExtents), XMVectorGetX(boxExtents));
        closestPoint.y = std::clamp(localSphereCenterFloat.y, -XMVectorGetY(boxExtents), XMVectorGetY(boxExtents));
        closestPoint.z = std::clamp(localSphereCenterFloat.z, -XMVectorGetZ(boxExtents), XMVectorGetZ(boxExtents));

        // 가장 가까운 점과 스피어 중심 간 거리 계산
        XMVECTOR closestPointVec = XMLoadFloat3(&closestPoint);
        float distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(closestPointVec, localSphereCenter)));
        depth = _boundingSphere.Radius - distance;
    }
    if (depth < 0.0f)
        depth = 0.0f;
    return depth;
}
#ifdef _DEBUG
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