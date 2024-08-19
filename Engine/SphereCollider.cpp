#include "pch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
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
    ColliderType type = other->GetColliderType();

    switch (type) {
    case ColliderType::Sphere:

        return _boundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());

    case ColliderType::Box:

        return _boundingSphere.Intersects(dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox());
    }

    return false;
}

#ifdef _DEBUG
void SphereCollider::CreateMesh() {
    _mesh = GET_SINGLETON(Resources)->LoadSphereMesh();
    _material = GET_SINGLETON(Resources)->Get<Material>(L"Collider")->Clone();
}

void SphereCollider::Render() {
    if (_mesh == nullptr || _material == nullptr)
        CreateMesh();

    GetTransform()->PushData();
    _material->PushGraphicsData();
    _mesh->Render();
}
#endif