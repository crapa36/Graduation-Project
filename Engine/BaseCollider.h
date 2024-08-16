#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
	Box,

};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual bool Intersects(shared_ptr<GameObject> gameObject) = 0;

	ColliderType GetColliderType() { return _colliderType; }
private:
	ColliderType _colliderType = {};
};