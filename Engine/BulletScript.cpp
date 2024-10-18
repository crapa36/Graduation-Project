#include "pch.h"
#include "BulletScript.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Input.h"
#include "Rigidbody.h"
#include "Scene.h"

BulletScript::BulletScript()
{
}

BulletScript::~BulletScript()
{
}

void BulletScript::LateUpdate()
{
	shared_ptr<Transform> parent = GetTransform()->GetParent().lock();

	if (INPUT->IsKeyJustPressed(DIK_G)) {
		GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		GetRigidbody()->SetVelocity(parent->GetLook() * _speed);
		GetGameObject()->SetEnable(true);
	}
}
