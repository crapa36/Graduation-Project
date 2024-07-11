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
    _shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI ���̾�� �׸��ڿ� ���Խ�Ű�� ����.
}

Light::~Light() {
}

void Light::FinalUpdate() {

    // ����Ʈ�� ��ġ�� ������Ʈ��.
    _lightInfo.position = GetTransform()->GetWorldPosition();

    // �׸��� ī�޶��� ��ġ�� ȸ��, ũ�⸦ ����Ʈ�� ����ȭ��.
    _shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
    _shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
    _shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

    // �׸��� ī�޶��� ���� ������Ʈ�� ȣ����.
    _shadowCamera->FinalUpdate();
}

void Light::Render() {
    assert(_lightIndex >= 0); // ����Ʈ �ε����� ��ȿ���� Ȯ����.

    // ��ȯ ������ GPU�� ������.
    GetTransform()->PushData();

    if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL) {

        // ���⼺ ����Ʈ�� ��� �׸��� �ؽ�ó�� ������.
        shared_ptr<Texture> shadowTex = GET_SINGLETON(Resources)->Get<Texture>(L"ShadowTarget");
        _lightMaterial->SetTexture(2, shadowTex);

        // ���������� ����� ����Ͽ� ������ ������.
        Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();
        _lightMaterial->SetMatrix(0, matVP);
    }
    else {

        // ����Ʈ ����Ʈ�� ���� ����Ʈ�� ��� ����Ʈ�� ������ ���� ũ�⸦ ������.
        float scale = 2 * _lightInfo.range;
        GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
    }

    // ����Ʈ �ε����� ������ ������.
    _lightMaterial->SetInt(0, _lightIndex);

    // ���� ������ GPU�� ������.
    _lightMaterial->PushGraphicsData();

    // ���� �޽ø� ��������.
    _volumeMesh->Render();
}

void Light::RenderShadow() {

    // �׸��� ������Ʈ�� �����ϰ� �׸��ڸ� ��������.
    _shadowCamera->GetCamera()->SortShadowObject();
    _shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction) {

    // ����Ʈ ������ ����ȭ��.
    direction.Normalize();

    // ����Ʈ ������ ������ ������.
    _lightInfo.direction = direction;

    // ����Ʈ�� ������ �ٶ󺸵��� ��ȯ��.
    GetTransform()->LookAt(direction);
}

void Light::SetLightType(LIGHT_TYPE type) {

    // ����Ʈ Ÿ���� ������.
    _lightInfo.lightType = static_cast<int32>(type);

    switch (type) {
    case LIGHT_TYPE::DIRECTIONAL:

        // ���⼺ ����Ʈ�� ��� ���簢�� �޽ÿ� ������ ������.
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Rectangle");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"DirLight");

        // ���� ���� ������ ������.
        _shadowCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
        _shadowCamera->GetCamera()->SetFar(10000.f); // �� ���������� �� �Ÿ� ����
        _shadowCamera->GetCamera()->SetNear(1.f); // �� ���������� ����� �Ÿ� ����
        _shadowCamera->GetCamera()->SetScale(0.2f);
        _shadowCamera->GetCamera()->SetWidth(4096);
        _shadowCamera->GetCamera()->SetHeight(4096);
        break;
    case LIGHT_TYPE::POINT:

        // ����Ʈ ����Ʈ�� ��� �� �޽ÿ� ������ ������.
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Sphere");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"PointLight");
        break;
    case LIGHT_TYPE::SPOT:

        // ���� ����Ʈ�� ��� �� �޽ÿ� ������ ������. (���� ���� ����Ʈ ���� ������ �߰��� �� ����)
        _volumeMesh = GET_SINGLETON(Resources)->Get<Mesh>(L"Sphere");
        _lightMaterial = GET_SINGLETON(Resources)->Get<Material>(L"PointLight");
        break;
    }
}