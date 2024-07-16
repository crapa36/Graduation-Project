#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"
#include "Scene.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT) {
    _shadowCamera = make_shared<GameObject>();
    _shadowCamera->AddComponent(make_shared<Transform>());
    _shadowCamera->AddComponent(make_shared<Camera>());
    uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
    _shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
}

Light::~Light() {
}

void Light::FinalUpdate() {

    // 라이트의 위치를 업데이트함.
    _lightInfo.position = GetTransform()->GetWorldPosition();

    // 그림자 카메라의 위치와 회전, 크기를 라이트와 동기화함.
    if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL) {
        shared_ptr<Camera> mainCamera = GET_SINGLETON(SceneManager)->GetActiveScene()->GetMainCamera();
        if (mainCamera != nullptr) {

            // 메인 카메라의 위치와 방향 가져오기
            Vec3 mainCameraPosition = mainCamera->GetTransform()->GetWorldPosition();
            Vec3 mainCameraDirection = mainCamera->GetTransform()->GetLook();

            // 그림자 카메라의 위치 설정 (메인 카메라 방향의 반대쪽에 배치)
            Vec3 shadowCameraPosition = mainCameraPosition + mainCameraDirection * 1.0f; // someDistance는 적절한 거리입니다.

            // 뷰 매트릭스 설정
            Matrix lightViewMatrix = XMMatrixLookToLH(shadowCameraPosition, mainCameraDirection, Vec3(0, 1, 0));

            // 그림자 카메라 설정
            _shadowCamera->GetCamera()->SetViewMatrix(lightViewMatrix);
            _shadowCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC); // Orthographic Projection 설정
            _shadowCamera->GetTransform()->SetLocalPosition(shadowCameraPosition);
            _shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
            _shadowCamera->GetTransform()->SetLocalScale(Vec3(1, 1, 1));
        }
    }
    else {

        // 비방향성 라이트의 경우, 위치, 회전, 크기를 라이트와 동기화합니다.
        _shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
        _shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
        _shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());
    }

    // 그림자 카메라의 최종 업데이트를 호출합니다.
    _shadowCamera->FinalUpdate();
}

void Light::Render() {
    assert(_lightIndex >= 0);

    GetTransform()->PushData();

    if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL) {
        shared_ptr<Texture> shadowTex = GET_SINGLETON(Resources)->Get<Texture>(L"ShadowTarget");
        _lightMaterial->SetTexture(2, shadowTex);

        Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
        _lightMaterial->SetMatrix(0, matVP);
    }
    else {
        float scale = 2 * _lightInfo.range;
        GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
    }

    _lightMaterial->SetInt(0, _lightIndex);
    _lightMaterial->PushGraphicsData();

    _volumeMesh->Render();
}

void Light::RenderShadow() {
    _shadowCamera->GetCamera()->SortShadowObject();
    _shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction) {
    direction.Normalize();

    _lightInfo.direction = direction;

    GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type) {
    _lightInfo.lightType = static_cast<int32>(type);

    switch (type) {
    case LIGHT_TYPE::DIRECTIONAL:
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Rectangle");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"DirLight");

        _shadowCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);

        // 직교 투영을 위한 설정
        _shadowCamera->GetCamera()->SetFar(5000.f); // 뷰 프러스텀의 먼 거리 설정
        _shadowCamera->GetCamera()->SetNear(-500.f); // 뷰 프러스텀의 가까운 거리 설정

        _shadowCamera->GetCamera()->SetScale(0.2f);
        _shadowCamera->GetCamera()->SetWidth(4096);
        _shadowCamera->GetCamera()->SetHeight(4096);

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