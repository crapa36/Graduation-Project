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

    shared_ptr<Texture> heightMap = GET_SINGLETON(Resources)->Load<Texture>(L"HeightMap", L"..\\Resources\\Texture\\Terrain\\height.png");
    Vec2 v = Vec2(heightMap->GetWidth(), heightMap->GetHeight());
    _material->SetVec2(0, Vec2(heightMap->GetWidth(), heightMap->GetHeight()));
    _material->SetVec2(1, Vec2(1500.f, 5000.f));
    _material->SetTexture(2, heightMap);

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