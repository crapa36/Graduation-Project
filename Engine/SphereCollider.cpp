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

    // �ٸ� �浹 �������� Ÿ���� ������
    ColliderType type = other->GetColliderType();

    // Ÿ�Կ� ���� ������ ���� �Ǵ� ���� ����
    switch (type) {
    case ColliderType::Sphere:

        // �ٸ� �浹 �����Ⱑ ��ü�� ���
        return _boundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());

    case ColliderType::AABB:

        // �ٸ� �浹 �����Ⱑ AABB�� ���
        return _boundingSphere.Intersects(dynamic_pointer_cast<AABBBoxCollider>(other)->GetBoundingBox());
    case ColliderType::OBB:

        // �ٸ� �浹 �����Ⱑ OBB�� ���
        return _boundingSphere.Intersects(dynamic_pointer_cast<OBBBoxCollider>(other)->GetBoundingBox());
    }

    // ���� ��쿡 �ش����� �ʴ� ���, �������� �ʴ� ������ ó��
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