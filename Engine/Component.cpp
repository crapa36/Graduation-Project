#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "BaseCollider.h"
#include "Rigidbody.h"

Component::Component(COMPONENT_TYPE type) : Object(OBJECT_TYPE::COMPONENT), _type(type) {
}

Component::~Component() {
}

shared_ptr<GameObject> Component::GetGameObject() {
    return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform() {
    return _gameObject.lock()->GetTransform();
}

shared_ptr<MeshRenderer> Component::GetMeshRenderer() {
    return _gameObject.lock()->GetMeshRenderer();
}

shared_ptr<Animator> Component::GetAnimator() {
    return _gameObject.lock()->GetAnimator();
}

shared_ptr<BaseCollider> Component::GetCollider() {
    return _gameObject.lock()->GetCollider();
}

shared_ptr<Rigidbody> Component::GetRigidbody() {
    return _gameObject.lock()->GetRigidbody();
}