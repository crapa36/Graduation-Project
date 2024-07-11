#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "SceneManager.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT) {
    _shadowCamera = make_shared<GameObject>();
    _shadowCamera->AddComponent(make_shared<Transform>());
    _shadowCamera->AddComponent(make_shared<Camera>());
    uint8 layerIndex = GET_SINGLETON(SceneManager)->LayerNameToIndex(L"UI");
    _shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI 레이어는 그림자에 포함시키지 않음.
}

Light::~Light() {
}

void Light::FinalUpdate() {

    // 라이트의 위치를 업데이트함.
    _lightInfo.position = GetTransform()->GetWorldPosition();

    // 그림자 카메라의 위치와 회전, 크기를 라이트와 동기화함.
    _shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
    _shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
    _shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

    // 그림자 카메라의 최종 업데이트를 호출함.
    _shadowCamera->FinalUpdate();
}

void Light::Render() {
    assert(_lightIndex >= 0); // 라이트 인덱스가 유효한지 확인함.

    // 변환 정보를 GPU로 전송함.
    GetTransform()->PushData();

    if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL) {

        // 방향성 라이트의 경우 그림자 텍스처를 설정함.
        shared_ptr<Texture> shadowTex = GET_SINGLETON(Resources)->Get<Texture>(L"ShadowTarget");
        _lightMaterial->SetTexture(2, shadowTex);

        // 뷰프로젝션 행렬을 계산하여 재질에 설정함.
        Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
        _lightMaterial->SetMatrix(0, matVP);
    }
    else {

        // 포인트 라이트와 스폿 라이트의 경우 라이트의 범위에 따라 크기를 조정함.
        float scale = 2 * _lightInfo.range;
        GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
    }

    // 라이트 인덱스를 재질에 설정함.
    _lightMaterial->SetInt(0, _lightIndex);

    // 재질 정보를 GPU로 전송함.
    _lightMaterial->PushGraphicsData();

    // 볼륨 메시를 렌더링함.
    _volumeMesh->Render();
}

void Light::RenderShadow() {

    // 그림자 오브젝트를 정렬하고 그림자를 렌더링함.
    _shadowCamera->GetCamera()->SortShadowObject();
    _shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction) {

    // 라이트 방향을 정규화함.
    direction.Normalize();

    // 라이트 정보에 방향을 설정함.
    _lightInfo.direction = direction;

    // 라이트의 방향을 바라보도록 변환함.
    GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type) {

    // 라이트 타입을 설정함.
    _lightInfo.lightType = static_cast<int32>(type);

    switch (type) {
    case LIGHT_TYPE::DIRECTIONAL:

        // 방향성 라이트의 경우 직사각형 메시와 재질을 설정함.
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Rectangle");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"DirLight");

        // 직교 투영 설정을 적용함.
        _shadowCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
        _shadowCamera->GetCamera()->SetFar(10000.f); // 뷰 프러스텀의 먼 거리 설정
        _shadowCamera->GetCamera()->SetNear(1.f); // 뷰 프러스텀의 가까운 거리 설정
        _shadowCamera->GetCamera()->SetScale(0.2f);
        _shadowCamera->GetCamera()->SetWidth(4096);
        _shadowCamera->GetCamera()->SetHeight(4096);
        break;
    case LIGHT_TYPE::POINT:

        // 포인트 라이트의 경우 구 메시와 재질을 설정함.
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Sphere");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"PointLight");
        break;
    case LIGHT_TYPE::SPOT:

        // 스폿 라이트의 경우 구 메시와 재질을 설정함. (추후 스폿 라이트 전용 설정이 추가될 수 있음)
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Sphere");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"PointLight");
        break;
    }
}