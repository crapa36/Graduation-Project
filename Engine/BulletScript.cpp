#include "pch.h"
#include "BulletScript.h"
#include "GameObject.h"
#include "Timer.h"
#include "Rigidbody.h"

BulletScript::BulletScript() {
}

BulletScript::~BulletScript() {
}

void BulletScript::LateUpdate() {
    _lifeTime -= DELTA_TIME;
    if (GetRigidbody()->GetVelocity().Length() < 1.f) {
        GetGameObject()->SetEnable(FALSE);
        _lifeTime = 10.f;
    }
    else if (_lifeTime <= 0) {
        GetGameObject()->SetEnable(FALSE);
        _lifeTime = 10.f;
    }
}