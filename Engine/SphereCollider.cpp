#include "pch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::FinalUpdate()
{
	_boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

	Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	_boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(shared_ptr<GameObject> gameObject)
{
	ColliderType colliderType = gameObject->GetCollider()->GetColliderType();
	if (colliderType == ColliderType::Sphere) {
		BoundingSphere sphere = dynamic_pointer_cast<SphereCollider>(gameObject->GetCollider())->GetBoundingSphere();
		return _boundingSphere.Intersects(sphere);
	}

	if (colliderType == ColliderType::Box) {
		BoundingBox box = dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider())->GetBoundingBox();
		return _boundingSphere.Intersects(box);
	}
}