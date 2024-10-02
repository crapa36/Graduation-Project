#pragma once
#include "Transform.h"

struct CMeshMaterialInfo {
    Vec4			diffuse;
    Vec4			ambient;
    Vec4			specular;
    wstring			name;
    char diffuseTexName[64] = "null";
    char normalTexName[64] = "null";
    char specularTexName[64] = "null";
    char detailDiffuseTexName[64] = "null";
    char detailNormalTexName[64]= "null";
    BYTE strlenD = 64;
    BYTE strlenN = 64;
    BYTE strlenS = 64;
    BYTE strlenDD = 64;
    BYTE strlenDN = 64;
};


#define VERTEXT_POSITION				0x01
#define VERTEXT_COLOR					0x02
#define VERTEXT_NORMAL					0x04
#define VERTEXT_TANGENT					0x08
#define VERTEXT_TEXTURE_COORD0			0x10
#define VERTEXT_TEXTURE_COORD1			0x20

#define VERTEXT_TEXTURE					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_DETAIL					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TEXTURE			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_TANGENT_TEXTURE	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_DETAIL			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TANGENT__DETAIL	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
struct CMeshLoadInfo
{
    char							m_pstrMeshName[256] = { 0 };

    UINT							m_nType = 0x00;

    XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

    int								m_nVertices = 0;
    int       nPositions = 0;
    XMFLOAT3* m_pxmf3Positions = NULL;
    int       nColors = 0;
    XMFLOAT4* m_pxmf4Colors = NULL;
    int       nNormals = 0;
    XMFLOAT3* m_pxmf3Normals = NULL;

    int								nTextureCoords = 0;
    XMFLOAT2* m_pxmf2TextureCoords0 = NULL;
    XMFLOAT2* m_pxmf2TextureCoords1 = NULL;

    int								nTangents = 0;
    int								nBiTangents = 0;
    XMFLOAT3* m_pxmf3Tangents = NULL;
    XMFLOAT3* m_pxmf3BiTangents = NULL;

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
    shared_ptr<Transform>               transform;
};


class CMeshLoader {
public:
    CMeshLoader();
    ~CMeshLoader();

public:
    void LoadBIN(const wstring& path);

   CMeshInfo Li2i(CMeshLoadInfo loadInfo);

public:
    int32 GetMeshCount() { return static_cast<int32>(_meshes.size()); }
    const CMeshInfo& GetMesh(int32 idx) { return _meshes[idx]; }

private:
    void LoadMesh(FILE* pInFile, CMeshInfo* info);
    void LoadMaterial(FILE* pInFile, CMeshInfo*info);

    CMeshInfo LoadFrameHierarchy(FILE* pInFile);
    void LoadGeometry(const wstring& pstrFileName);

    void CreateTextures();
    void CreateMaterials();

private:
    wstring			_resourceDirectory;

    int32                               _count = 0;
    vector<int32>                       _parentCount;
    vector<int32>                       _childCount;

    vector<CMeshInfo>					_meshes;
};
