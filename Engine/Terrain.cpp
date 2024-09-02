#include "pch.h"
#include "Terrain.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"

Terrain::Terrain() : Component(COMPONENT_TYPE::TERRAIN) {
}

Terrain::~Terrain() {
}

void Terrain::Init(int32 sizeX, int32 sizeZ) {
    _sizeX = sizeX;
    _sizeZ = sizeZ;

    _material = GET_SINGLETON(Resources)->Get<Material>(L"Terrain");

    _material->SetInt(1, _sizeX);
    _material->SetInt(2, _sizeZ);
    _material->SetFloat(0, _maxTesselation);

    _heightMap = GET_SINGLETON(Resources)->Load<Texture>(L"HeightMap", L"..\\Resources\\Texture\\Terrain\\height.png");
    Vec2 v = Vec2(_heightMap->GetWidth(), _heightMap->GetHeight());
    _material->SetVec2(0, Vec2(_heightMap->GetWidth(), _heightMap->GetHeight()));
    _material->SetVec2(1, Vec2(1500.f, 5000.f)); // �ּ�/ �ִ� �׼����̼� �Ÿ�
    _material->SetTexture(2, _heightMap);

    shared_ptr<MeshRenderer> meshRenderer = GetGameObject()->GetMeshRenderer();
    {
        shared_ptr<Mesh> mesh = GET_SINGLETON(Resources)->LoadTerrainMesh(sizeX, sizeZ);
        meshRenderer->SetMesh(mesh);
    }
    {
        shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Terrain");
        meshRenderer->SetMaterial(material);
    }
}

void Terrain::FinalUpdate() {
    shared_ptr<Camera> mainCamera = GET_SINGLETON(SceneManager)->GetActiveScene()->GetMainCamera();
    if (mainCamera == nullptr)
        return;

    Vec3 pos = mainCamera->GetTransform()->GetLocalPosition();
    _material->SetVec4(0, Vec4(pos.x, pos.y, pos.z, 0));
}

float Terrain::GetHeightAtPosition(float x, float z) const {
    if (!_heightMap)
        return 0.0f;

    // ���� ���� ũ��
    int32 width = _heightMap->GetWidth();
    int32 height = _heightMap->GetHeight();

    // �ؽ�ó ��ǥ�� ��ȯ
    int32 texX = static_cast<int32>(x * width / (_sizeX * 50));
    int32 texZ = static_cast<int32>(height - (z * height / (_sizeZ * 50)));

    // �ؽ�ó ��ǥ�� ��ȿ���� Ȯ��
    if (texX < 0 || texX >= width || texZ < 0 || texZ >= height)
        return 0.0f;

    // ���� �ʿ��� ���� ���� ������
    float heightValue = _heightMap->GetPixel(texX, texZ).x; // ���� ���� ������ ä���� ����Ѵٰ� ����

    // ���� ���� ���� ���̷� ��ȯ
    return heightValue; // _maxHeight�� ������ �ִ� ����
}