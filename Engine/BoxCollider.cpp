#include "pch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Resources.h"

BoxCollider::BoxCollider() : BaseCollider(ColliderType::Box) {
}

BoxCollider::~BoxCollider() {
}

void BoxCollider::FinalUpdate() {
    _boundingBox.Center = GetGameObject()->GetTransform()->GetWorldPosition();
    _boundingBox.Extents = _extents;
}

bool BoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {
    return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

bool BoxCollider::Intersects(const shared_ptr<BaseCollider>& other) {
    ColliderType type = other->GetColliderType();

    switch (type) {
    case ColliderType::Sphere:

        return _boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());

    case ColliderType::Box:

        return _boundingBox.Intersects(dynamic_pointer_cast<BoxCollider>(other)->GetBoundingBox());
    }

    return false;
}

#ifdef _DEBUG
void BoxCollider::CreateMesh() {
    _mesh = GET_SINGLETON(Resources)->LoadCubeMesh();
    _material = GET_SINGLETON(Resources)->Get<Material>(L"Collider")->Clone();
}

void BoxCollider::Render() {
    if (_mesh == nullptr || _material == nullptr)
        CreateMesh();

    GetTransform()->PushData();
    _material->PushGraphicsData();
    _mesh->Render();
}
#endif