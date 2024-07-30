#pragma once

struct CMeshMaterialInfo {
    Vec4			diffuse;
    Vec4			ambient;
    Vec4			specular;
    wstring			name;
    wstring			diffuseTexName;
    wstring			normalTexName;
    wstring			specularTexName;
};


#define VERTEXT_POSITION			0x01
#define VERTEXT_COLOR				0x02
#define VERTEXT_NORMAL				0x04
class CMeshLoadInfo
{
public:
    CMeshLoadInfo() { }
    ~CMeshLoadInfo();

public:
    char							m_pstrMeshName[256] = { 0 };

    UINT							m_nType = 0x00;

    XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

    int								m_nVertices = 0;
    XMFLOAT3* m_pxmf3Positions = NULL;
    XMFLOAT4* m_pxmf4Colors = NULL;
    XMFLOAT3* m_pxmf3Normals = NULL;

    int								m_nIndices = 0;
    UINT* m_pnIndices = NULL;

    int								m_nSubMeshes = 0;
    int* m_pnSubSetIndices = NULL;
    UINT** m_ppnSubSetIndices = NULL;
};


struct CMeshInfo
{
    string								name;
    vector<Vertex>						vertices;
    vector<vector<uint32>>				indices;
    vector<CMeshMaterialInfo>			materials;
};


class CMeshLoader {
public:
    CMeshLoader();
    ~CMeshLoader();

public:
    void LoadBIN(const wstring& path);

public:
    int32 GetMeshCount() { return static_cast<int32>(_meshes.size()); }
    const CMeshInfo& GetMesh(int32 idx) { return _meshes[idx]; }
private:
    CMeshLoadInfo LoadMesh(FILE* pInFile);
    CMeshMaterialInfo LoadMaterial(FILE* pInFile);

    void LoadFrameHierarchy(FILE* pInFile);
    void LoadGeometry(wstring pstrFileName);

    void LoadInfoToInfo();

    void CreateTextures();
    void CreateMaterials();

private:
    wstring			_resourceDirectory;

    vector<CMeshInfo>					_meshes;
    vector<class Transform>             _transform;

    vector<CMeshLoadInfo>               _loadMeshes;
};
