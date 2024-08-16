#include "pch.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Resources.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere) {
}

SphereCollider::~SphereCollider() {
}

void SphereCollider::FinalUpdate() {
    _boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

    Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
    _boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {
    return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(const shared_ptr<BaseCollider>& other) {

    // 다른 충돌 감지기의 타입을 가져옴
    ColliderType type = other->GetColliderType();

    // 타입에 따라 적절한 교차 판단 로직 수행
    switch (type) {
    case ColliderType::Sphere:

        // 다른 충돌 감지기가 구체인 경우
        return _boundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());

    case ColliderType::AABB:

        // 다른 충돌 감지기가 AABB인 경우
        return _boundingSphere.Intersects(dynamic_pointer_cast<AABBBoxCollider>(other)->GetBoundingBox());
    case ColliderType::OBB:

        // 다른 충돌 감지기가 OBB인 경우
        return _boundingSphere.Intersects(dynamic_pointer_cast<OBBBoxCollider>(other)->GetBoundingBox());
    }

    // 위의 경우에 해당하지 않는 경우, 교차하지 않는 것으로 처리
    return false;
}

#ifdef _DEBUG
void SphereCollider::CreateMesh() {
    _mesh = GET_SINGLETON(Resources)->LoadSphereMesh();
    _material = GET_SINGLETON(Resources)->Get<Material>(L"Collider")->Clone();
}

void SphereCollider::Render() {
    if (_meshRenderer == nullptr)
        CreateMesh();

    GetTransform()->PushData();
    _material->PushGraphicsData();
    _mesh->Render();
}
#endif