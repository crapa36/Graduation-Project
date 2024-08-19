#include "pch.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

BoxCollider::BoxCollider() : BaseCollider(ColliderType::Box)
{
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::FinalUpdate()
{
	_boundingBox.Center = GetGameObject()->GetTransform()->GetWorldPosition();
	_boundingBox.Extents = _extents;
}

bool BoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

bool BoxCollider::Intersects(shared_ptr<GameObject> gameObject)
{
	ColliderType colliderType = gameObject->GetCollider()->GetColliderType();
	if (colliderType == ColliderType::Sphere) {
		BoundingSphere sphere = dynamic_pointer_cast<SphereCollider>(gameObject->GetCollider())->GetBoundingSphere();
		return _boundingBox.Intersects(sphere);
	}

	if (colliderType == ColliderType::Box) {
		BoundingBox box = dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider())->GetBoundingBox();
		return _boundingBox.Intersects(box);
	}
}
