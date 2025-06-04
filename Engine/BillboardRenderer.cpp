#include "pch.h"
#include "BillboardRenderer.h"
#include "Resources.h"
#include "Transform.h"

BillboardRenderer::BillboardRenderer()
    : Component(COMPONENT_TYPE::BILLBOARD_RENDERER) {
    _mesh = GResources->LoadPointMesh();
    _material = GResources->Get<Material>(L"Billboard");
        shared_ptr<Texture> tex = GResources->Load<Texture>(L"BillboardTexture", L"..\\Resources\\Texture\\TreeBillboard.png");
        _material->SetTexture(0, tex);

}

BillboardRenderer::~BillboardRenderer() {}

void BillboardRenderer::SetTexture(const wstring& texturePath) {
    shared_ptr<Texture> texture = GResources->Load<Texture>(L"BillboardTexture", texturePath);
    _material->SetTexture(0, texture);
}

void BillboardRenderer::SetAxisConstraint(const Vec3& axis) {
    _axisConstraint = axis;
    _axisConstraint.Normalize();
}

void BillboardRenderer::FinalUpdate() {
    // Transform 데이터를 업데이트
    shared_ptr<Transform> transform = GetTransform();
    transform->FinalUpdate();
}

void BillboardRenderer::Render() {

    // Transform 데이터를 셰이더로 푸시
    GetTransform() ->PushData();

    // 추가적인 데이터 (축 제한) 전달
    Vec4 axisConstraint = { _axisConstraint.x, _axisConstraint.y, _axisConstraint.z, 0.0f };
    _material->SetVec4(0, axisConstraint); // 축 제한 전달

    Vec4 scale = Vec4(GetTransform()->GetLocalScale().x, GetTransform()->GetLocalScale().y, GetTransform()->GetLocalScale().z,0.0f);
    _material->SetVec4(1, scale); // 텍스쳐 좌표
    _material->PushGraphicsData();

    // 메쉬 렌더링
    _mesh->Render();
}
