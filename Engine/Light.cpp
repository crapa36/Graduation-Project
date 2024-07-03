#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT) {
}

Light::~Light() {
}

void Light::FinalUpdate() {
    _lightInfo.position = GetTransform()->GetWorldPosition();
}

void Light::Render() {
    assert(_lightIndex >= 0);

    GetTransform()->PushData();

    _lightMaterial->SetInt(0, _lightIndex);
    _lightMaterial->PushData();

    switch (static_cast<LIGHT_TYPE>(_lightInfo.lightType)) {
    case LIGHT_TYPE::POINT:
    case LIGHT_TYPE::SPOT:
        float scale = 2 * _lightInfo.range;
        GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
        break;
    }

    _volumeMesh->Render();
}

void Light::SetLightType(LIGHT_TYPE type) {
    _lightInfo.lightType = static_cast<int32>(type);

    switch (type) {
    case LIGHT_TYPE::DIRECTIONAL:
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Rectangle");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"DirLight");
        break;
    case LIGHT_TYPE::POINT:
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Sphere");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"PointLight");
        break;
    case LIGHT_TYPE::SPOT:
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Sphere");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"PointLight");
        break;
    }
}