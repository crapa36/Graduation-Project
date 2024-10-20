#include "pch.h"
#include "Resources.h"
#include "Engine.h"
#include "MeshData.h"
#include "Camera.h"
#include "Frustum.h"
#include "Mesh.h"

void Resources::Init() {
    CreateDefaultShader();
    CreateDefaultMaterial();
}

shared_ptr<Mesh> Resources::LoadPointMesh() {
    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Point");
    if (findMesh)
        return findMesh;

    vector<Vertex> vec(1);
    vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

    vector<uint32> idx(1);
    idx[0] = 0;

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Create(vec, idx);
    Add(L"Point", mesh);

    return mesh;
}

shared_ptr<Mesh> Resources::LoadCameraFrustumMesh(shared_ptr<Camera> camera) {

    // �������� �������� �����ɴϴ�.
    vector<Vec3> worldPos = camera->GetFrustum().GetWorldPos();

    vector<Vertex> vec(24);

    //�ո�
    vec[0] = Vertex(worldPos[0], Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[1] = Vertex(worldPos[1], Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[2] = Vertex(worldPos[2], Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[3] = Vertex(worldPos[3], Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

    // �޸�
    vec[4] = Vertex(worldPos[4], Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[5] = Vertex(worldPos[5], Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[6] = Vertex(worldPos[6], Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[7] = Vertex(worldPos[7], Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));

    // ����
    vec[8] = Vertex(worldPos[1], Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[9] = Vertex(worldPos[5], Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[10] = Vertex(worldPos[6], Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[11] = Vertex(worldPos[2], Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));

    // �Ʒ���
    vec[12] = Vertex(worldPos[0], Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[13] = Vertex(worldPos[3], Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[14] = Vertex(worldPos[7], Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[15] = Vertex(worldPos[4], Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));

    // ���ʸ�
    vec[16] = Vertex(worldPos[4], Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[17] = Vertex(worldPos[7], Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[18] = Vertex(worldPos[3], Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[19] = Vertex(worldPos[0], Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));

    // �����ʸ�
    vec[20] = Vertex(worldPos[2], Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[21] = Vertex(worldPos[6], Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[22] = Vertex(worldPos[5], Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[23] = Vertex(worldPos[1], Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vector<uint32> idx(36);

    // �鿡 ���� �ε��� ����
    // �ո�
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    // �޸�
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;

    // ����
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;

    // �Ʒ���
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;

    // ���ʸ�
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;

    // �����ʸ�
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;

    // �޽� ���� �� ��ȯ
    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Create(vec, idx);
    Add(L"Frustum", mesh);
    return mesh;
}

shared_ptr<Mesh> Resources::LoadRectangleMesh() {
    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Rectangle");
    if (findMesh)
        return findMesh;

    float w2 = 0.5f;
    float h2 = 0.5f;

    vector<Vertex> vec(4);

    // �ո�
    vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

    vector<uint32> idx(6);

    // �ո�
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Create(vec, idx);
    Add(L"Rectangle", mesh);

    return mesh;
}

shared_ptr<Mesh> Resources::LoadTerrainMesh(int32 sizeX, int32 sizeZ) {
    vector<Vertex> vec;

    for (int32 z = 0; z < sizeZ + 1; z++) {
        for (int32 x = 0; x < sizeX + 1; x++) {
            Vertex vtx;
            vtx.pos = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
            vtx.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
            vtx.normal = Vec3(0.f, 1.f, 0.f);
            vtx.tangent = Vec3(1.f, 0.f, 0.f);

            vec.push_back(vtx);
        }
    }

    vector<uint32> idx;

    for (int32 z = 0; z < sizeZ; z++) {
        for (int32 x = 0; x < sizeX; x++) {

            //  [0]
            //   |	\
			//  [2] - [1]
            idx.push_back((sizeX + 1) * (z + 1) + (x));
            idx.push_back((sizeX + 1) * (z)+(x + 1));
            idx.push_back((sizeX + 1) * (z)+(x));

            //  [1] - [2]
            //   	\  |
            //		  [0]
            idx.push_back((sizeX + 1) * (z)+(x + 1));
            idx.push_back((sizeX + 1) * (z + 1) + (x));
            idx.push_back((sizeX + 1) * (z + 1) + (x + 1));
        }
    }

    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Terrain");
    if (findMesh) {
        findMesh->Create(vec, idx);
        return findMesh;
    }

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Create(vec, idx);
    Add(L"Terrain", mesh);
    return mesh;
}

shared_ptr<Mesh> Resources::LoadCubeMesh() {
    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Cube");
    if (findMesh)
        return findMesh;

    float w2 = 0.5f;
    float h2 = 0.5f;
    float d2 = 0.5f;

    vector<Vertex> vec(24);

    // �ո�
    vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

    // �޸�
    vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));

    // ����
    vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));

    // �Ʒ���
    vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));

    // ���ʸ�
    vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));

    // �����ʸ�
    vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

    vector<uint32> idx(36);

    // �ո�
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    // �޸�
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;

    // ����
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;

    // �Ʒ���
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;

    // ���ʸ�
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;

    // �����ʸ�
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Create(vec, idx);
    Add(L"Cube", mesh);

    return mesh;
}

shared_ptr<Mesh> Resources::LoadSphereMesh() {
    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Sphere");
    if (findMesh)
        return findMesh;

    float radius = 0.5f; // ���� ������
    uint32 stackCount = 20; // ���� ����
    uint32 sliceCount = 20; // ���� ����

    vector<Vertex> vec;

    Vertex v;

    // �ϱ�
    v.pos = Vec3(0.0f, radius, 0.0f);
    v.uv = Vec2(0.5f, 0.0f);
    v.normal = v.pos;
    v.normal.Normalize();
    v.tangent = Vec3(1.0f, 0.0f, 1.0f);
    vec.push_back(v);

    float stackAngle = XM_PI / stackCount;
    float sliceAngle = XM_2PI / sliceCount;

    float deltaU = 1.f / static_cast<float>(sliceCount);
    float deltaV = 1.f / static_cast<float>(stackCount);

    // �������� ���鼭 ������ ����Ѵ� (�ϱ�/���� �������� ������ X)
    for (uint32 y = 1; y <= stackCount - 1; ++y) {
        float phi = y * stackAngle;

        // ������ ��ġ�� ����
        for (uint32 x = 0; x <= sliceCount; ++x) {
            float theta = x * sliceAngle;

            v.pos.x = radius * sinf(phi) * cosf(theta);
            v.pos.y = radius * cosf(phi);
            v.pos.z = radius * sinf(phi) * sinf(theta);

            v.uv = Vec2(deltaU * x, deltaV * y);

            v.normal = v.pos;
            v.normal.Normalize();

            v.tangent.x = -radius * sinf(phi) * sinf(theta);
            v.tangent.y = 0.0f;
            v.tangent.z = radius * sinf(phi) * cosf(theta);
            v.tangent.Normalize();

            vec.push_back(v);
        }
    }

    // ����
    v.pos = Vec3(0.0f, -radius, 0.0f);
    v.uv = Vec2(0.5f, 1.0f);
    v.normal = v.pos;
    v.normal.Normalize();
    v.tangent = Vec3(1.0f, 0.0f, 0.0f);
    vec.push_back(v);

    vector<uint32> idx(36);

    // �ϱ� �ε���
    for (uint32 i = 0; i <= sliceCount; ++i) {

        //  [0]
        //   |  \
		//  [i+1]-[i+2]
        idx.push_back(0);
        idx.push_back(i + 2);
        idx.push_back(i + 1);
    }

    // ���� �ε���
    uint32 ringVertexCount = sliceCount + 1;
    for (uint32 y = 0; y < stackCount - 2; ++y) {
        for (uint32 x = 0; x < sliceCount; ++x) {

            //  [y, x]-[y, x+1]
            //  |		/
            //  [y+1, x]
            idx.push_back(1 + (y)*ringVertexCount + (x));
            idx.push_back(1 + (y)*ringVertexCount + (x + 1));
            idx.push_back(1 + (y + 1) * ringVertexCount + (x));

            //		 [y, x+1]
            //		 /	  |
            //  [y+1, x]-[y+1, x+1]
            idx.push_back(1 + (y + 1) * ringVertexCount + (x));
            idx.push_back(1 + (y)*ringVertexCount + (x + 1));
            idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
        }
    }

    // ���� �ε���
    uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
    uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
    for (uint32 i = 0; i < sliceCount; ++i) {

        //  [last+i]-[last+i+1]
        //  |      /
        //  [bottom]
        idx.push_back(bottomIndex);
        idx.push_back(lastRingStartIndex + i);
        idx.push_back(lastRingStartIndex + i + 1);
    }

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Create(vec, idx);
    Add(L"Sphere", mesh);

    return mesh;
}

shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
                                             const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
                                             D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor) {
    shared_ptr<Texture> texture = make_shared<Texture>();
    texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor);
    Add(name, texture);

    return texture;
}

shared_ptr<Texture> Resources::CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D) {
    shared_ptr<Texture> texture = make_shared<Texture>();
    texture->CreateFromResource(tex2D);
    Add(name, texture);

    return texture;
}

shared_ptr<class MeshData> Resources::LoadFBX(const wstring& path) {
    wstring key = path;
    shared_ptr<MeshData> meshData = Get<MeshData>(key);
    if (meshData)
        return meshData;
    meshData = MeshData::LoadFromFBX(path);
    meshData->SetName(key);
    Add(key, meshData);

    return meshData;
}

shared_ptr<class MeshData> Resources::LoadBIN(const wstring& path) {
    wstring key = path;
    shared_ptr<MeshData> meshData = Get<MeshData>(key);
    if (meshData)
        return meshData;
    meshData = MeshData::LoadFromBIN(path);
    meshData->SetName(key);
    Add(key, meshData);

    return meshData;
}

void Resources::CreateDefaultShader() {

    // Skysphere
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::LESS_EQUAL
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\skysphere.fx", info);
        Add<Shader>(L"Skysphere", shader);
    }

    // Skybox
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::LESS_EQUAL
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\skybox.fx", info);
        Add<Shader>(L"Skybox", shader);
    }

    // Deferred (Deferred)
    {
        ShaderInfo info =
        {
            SHADER_TYPE::DEFERRED
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
        Add<Shader>(L"Deferred", shader);
    }

    // Forward (Forward)
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
        Add<Shader>(L"Forward", shader);
    }

    // Texture (Forward)
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
        };

        ShaderArg arg = {
            "VS_Tex",
            "",
            "",
            "",
            "PS_Tex"
        };
        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
        Add<Shader>(L"Texture", shader);
    }

    // DirLight
    {
        ShaderInfo info =
        {
            SHADER_TYPE::LIGHTING,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
            BLEND_TYPE::ONE_TO_ONE_BLEND
        };

        ShaderArg arg = {
            "VS_DirLight",
            "",
            "",
            "",
            "PS_DirLight"
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
        Add<Shader>(L"DirLight", shader);
    }

    // PointLight
    {
        ShaderInfo info =
        {
            SHADER_TYPE::LIGHTING,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::GREATER_EQUAL_NO_WRITE,
            BLEND_TYPE::ONE_TO_ONE_BLEND
        };

        ShaderArg arg = {
            "VS_PointLight",
            "",
            "",
            "",
            "PS_PointLight"
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
        Add<Shader>(L"PointLight", shader);
    }

    // Final
    {
        ShaderInfo info =
        {
            SHADER_TYPE::LIGHTING,
            RASTERIZER_TYPE::CULL_BACK,
            DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
        };

        ShaderArg arg = {
            "VS_Final",
            "",
            "",
            "",
            "PS_Final"
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
        Add<Shader>(L"Final", shader);
    }

    // Compute Shader
    {
        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateComputeShader(L"..\\Resources\\Shader\\compute.fx", "CS_Main", "cs_5_0");
        Add<Shader>(L"ComputeShader", shader);
    }

    // Particle
    {
        ShaderInfo info =
        {
            SHADER_TYPE::PARTICLE,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE,
            BLEND_TYPE::ALPHA_BLEND,
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST
        };

        ShaderArg arg = {
            "VS_Main",
            "",
            "",
            "GS_Main",
            "PS_Main"
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\particle.fx", info, arg);
        Add<Shader>(L"Particle", shader);
    }

    // ComputeParticle
    {
        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateComputeShader(L"..\\Resources\\Shader\\particle.fx", "CS_Main", "cs_5_0");
        Add<Shader>(L"ComputeParticle", shader);
    }

    // Shadow
    {
        ShaderInfo info =
        {
            SHADER_TYPE::SHADOW,
            RASTERIZER_TYPE::CULL_BACK,
            DEPTH_STENCIL_TYPE::LESS,
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\shadow.fx", info);
        Add<Shader>(L"Shadow", shader);
    }

    // Tessellation
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::WIREFRAME,
            DEPTH_STENCIL_TYPE::LESS,
            BLEND_TYPE::DEFAULT,
            D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
        };

        ShaderArg arg =
        {
            "VS_Main",
            "HS_Main",
            "DS_Main",
            "",
            "PS_Main",
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\tessellation.fx", info, arg);
        Add<Shader>(L"Tessellation", shader);
    }

    // Terrain
    {
        ShaderInfo info =
        {
            SHADER_TYPE::DEFERRED,
            RASTERIZER_TYPE::CULL_BACK,
            DEPTH_STENCIL_TYPE::LESS,
            BLEND_TYPE::DEFAULT,
            D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
        };

        ShaderArg arg =
        {
            "VS_Main",
            "HS_Main",
            "DS_Main",
            "",
            "PS_Main",
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\terrain.fx", info, arg);
        Add<Shader>(L"Terrain", shader);
    }

    // ComputeAnimation
    {
        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateComputeShader(L"..\\Resources\\Shader\\animation.fx", "CS_Main", "cs_5_0");
        Add<Shader>(L"ComputeAnimation", shader);
    }

    // Frustum
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::WIREFRAME,
            DEPTH_STENCIL_TYPE::LESS,
            BLEND_TYPE::DEFAULT,
            D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
        Add<Shader>(L"Frustum", shader);
    }

    // Collider
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::WIREFRAME,
            DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,

        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
        Add<Shader>(L"Collider", shader);
    }
}

void Resources::CreateDefaultMaterial() {

    // Skysphere
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Skysphere");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Skysphere", material);
    }

    // Skysphere
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Skybox");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Skybox", material);
    }

    // DirLight
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"DirLight");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, GET_SINGLETON(Resources)->Get<Texture>(L"PositionTarget"));
        material->SetTexture(1, GET_SINGLETON(Resources)->Get<Texture>(L"NormalTarget"));
        Add<Material>(L"DirLight", material);
    }

    // PointLight
    {
        const WindowInfo& window = GEngine->GetWindow();
        Vec2 resolution = { static_cast<float>(window.clientWidth), static_cast<float>(window.clientHeight) };

        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"PointLight");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, GET_SINGLETON(Resources)->Get<Texture>(L"PositionTarget"));
        material->SetTexture(1, GET_SINGLETON(Resources)->Get<Texture>(L"NormalTarget"));
        material->SetVec2(0, resolution);
        Add<Material>(L"PointLight", material);
    }

    // Final
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Final");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, GET_SINGLETON(Resources)->Get<Texture>(L"DiffuseTarget"));
        material->SetTexture(1, GET_SINGLETON(Resources)->Get<Texture>(L"DiffuseLightTarget"));
        material->SetTexture(2, GET_SINGLETON(Resources)->Get<Texture>(L"SpecularLightTarget"));
        Add<Material>(L"Final", material);
    }

    // Compute Shader
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"ComputeShader");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"ComputeShader", material);
    }

    // Particle
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Particle");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Particle", material);
    }

    // ComputeParticle
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"ComputeParticle");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);

        Add<Material>(L"ComputeParticle", material);
    }

    // Leather
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Deferred");
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->Load<Texture>(L"Leather", L"..\\Resources\\Texture\\Leather.jpg");
        shared_ptr<Texture> texture2 = GET_SINGLETON(Resources)->Load<Texture>(L"Leather_Normal", L"..\\Resources\\Texture\\Leather_Normal.jpg");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        material->SetTexture(1, texture2);
        Add<Material>(L"Leather", material);
    }

    // Pebbles
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Deferred");
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->Load<Texture>(L"Pebbles", L"..\\Resources\\Texture\\Pebbles.jpg");
        shared_ptr<Texture> texture2 = GET_SINGLETON(Resources)->Load<Texture>(L"Pebbles_Normal", L"..\\Resources\\Texture\\Pebbles_Normal.jpg");

        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        material->SetTexture(1, texture2);

        Add<Material>(L"Pebbles", material);
    }

    // Stone_Floor
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Deferred");
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->Load<Texture>(L"Stone_Floor", L"..\\Resources\\Texture\\Stone_Floor.jpg");
        shared_ptr<Texture> texture2 = GET_SINGLETON(Resources)->Load<Texture>(L"Stone_Floor_Normal", L"..\\Resources\\Texture\\Stone_Floor_Normal.jpg");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        material->SetTexture(1, texture2);
        Add<Material>(L"Stone_Floor", material);
    }

    // Wood
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Deferred");
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->Load<Texture>(L"Wood", L"..\\Resources\\Texture\\Wood.jpg");
        shared_ptr<Texture> texture2 = GET_SINGLETON(Resources)->Load<Texture>(L"Wood_Normal", L"..\\Resources\\Texture\\Wood_Normal.jpg");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        material->SetTexture(1, texture2);
        Add<Material>(L"Wood", material);
    }

    // Shadow
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Shadow");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Shadow", material);
    }

    // Tessellation
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Tessellation");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Tessellation", material);
    }

    // Terrain
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Terrain");
        shared_ptr<Texture> texture = GET_SINGLETON(Resources)->Load<Texture>(L"Terrain", L"..\\Resources\\Texture\\Terrain\\pineforest02.dds");
        shared_ptr<Texture> normalTexture = GET_SINGLETON(Resources)->Load<Texture>(L"Terrain_Normal", L"..\\Resources\\Texture\\Terrain\\pineforest02_n.dds");
        shared_ptr<Texture> detailTexture = GET_SINGLETON(Resources)->Load<Texture>(L"Terrain_Detail", L"..\\Resources\\Texture\\Terrain\\terrain_detail.jpg");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        material->SetTexture(0, texture);
        material->SetTexture(1, normalTexture);

        //material->SetTexture(3, detailTexture);

        Add<Material>(L"Terrain", material);
    }

    // ComputeAnimation
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"ComputeAnimation");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"ComputeAnimation", material);
    }

    // Frustum
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Frustum");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Frustum", material);
    }

    // Collider
    {
        shared_ptr<Shader> shader = GET_SINGLETON(Resources)->Get<Shader>(L"Collider");
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(shader);
        Add<Material>(L"Collider", material);
    }
}