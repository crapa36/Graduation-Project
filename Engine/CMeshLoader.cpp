#include "pch.h"
#include "CMeshLoader.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Shader.h"
#include "Material.h"


//
    int ReadIntegerFromFile(FILE* pInFile) {
        int nValue = 0;
        UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
        return(nValue);
    }
    float ReadFloatFromFile(FILE* pInFile) {
        float fValue = 0;
        UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
        return(fValue);
    }
    BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken) {
        BYTE nStrLength = 0;
        UINT nReads = 0;
        nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
        nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
        pstrToken[nStrLength] = '\0';

        return(nStrLength);
    }

//

CMeshLoader::CMeshLoader() {
}

CMeshLoader::~CMeshLoader() {
}

void CMeshLoader::LoadBIN(const wstring& path) {
    _resourceDirectory = path;

    LoadGeometry(path);

    for (int i = 0; i < _childCount.size(); i++) {
        for (int j = 0; j < _childCount.at(i); j++) {
            if(i+j < _meshes.size())
                _meshes.at(i+j).transform->SetParent(_meshes.at(_parentCount.at(i)).transform);
        }
    }

    CreateTextures();
    CreateMaterials();
}

void CMeshLoader::LoadMesh(FILE* pInFile, CMeshInfo* info) {
    char pstrToken[64] = { '\0' };
    UINT nReads = 0;

    int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

    CMeshLoadInfo loadInfo;

    loadInfo.m_nVertices = ::ReadIntegerFromFile(pInFile);
    ::ReadStringFromFile(pInFile, loadInfo.m_pstrMeshName);

    for (; ; ) {
        ::ReadStringFromFile(pInFile, pstrToken);

        if (!strcmp(pstrToken, "<Bounds>:")) {
            nReads = (UINT)::fread(&(loadInfo.m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
            nReads = (UINT)::fread(&(loadInfo.m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
        }
        else if (!strcmp(pstrToken, "<Positions>:")) {
            loadInfo.nPositions = ::ReadIntegerFromFile(pInFile);

            if (loadInfo.nPositions > 0) {
                loadInfo.m_pxmf3Positions = new XMFLOAT3[loadInfo.nPositions];
                nReads = (UINT)::fread(loadInfo.m_pxmf3Positions, sizeof(XMFLOAT3), loadInfo.nPositions, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<Colors>:")) {
            nColors = ::ReadIntegerFromFile(pInFile);
            loadInfo.nColors = nColors;
            if (nColors > 0) {
                loadInfo.m_nType |= VERTEXT_COLOR;
                loadInfo.m_pxmf4Colors = new XMFLOAT4[nColors];
                nReads = (UINT)::fread(loadInfo.m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<TextureCoords0>:"))
        {
            nReads = (UINT)::fread(&loadInfo.nTextureCoords, sizeof(int), 1, pInFile);
            if (loadInfo.nTextureCoords > 0)
            {
                loadInfo.m_nType |= VERTEXT_TEXTURE_COORD0;
                loadInfo.m_pxmf2TextureCoords0 = new XMFLOAT2[loadInfo.nTextureCoords];
                nReads = (UINT)::fread(loadInfo.m_pxmf2TextureCoords0, sizeof(XMFLOAT2), loadInfo.nTextureCoords, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<TextureCoords1>:"))
        {
            nReads = (UINT)::fread(&loadInfo.nTextureCoords, sizeof(int), 1, pInFile);
            if (loadInfo.nTextureCoords > 0)
            {
                loadInfo.m_nType |= VERTEXT_TEXTURE_COORD1;
                loadInfo.m_pxmf2TextureCoords1 = new XMFLOAT2[loadInfo.nTextureCoords];
                nReads = (UINT)::fread(loadInfo.m_pxmf2TextureCoords1, sizeof(XMFLOAT2), loadInfo.nTextureCoords, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<Tangents>:"))
        {
            nReads = (UINT)::fread(&loadInfo.nTangents, sizeof(int), 1, pInFile);
            if (loadInfo.nTangents > 0)
            {
                loadInfo.m_nType |= VERTEXT_TANGENT;
                loadInfo.m_pxmf3Tangents = new XMFLOAT3[loadInfo.nTangents];
                nReads = (UINT)::fread(loadInfo.m_pxmf3Tangents, sizeof(XMFLOAT3), loadInfo.nTangents, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<BiTangents>:"))
        {
            nReads = (UINT)::fread(&loadInfo.nBiTangents, sizeof(int), 1, pInFile);
            if (loadInfo.nBiTangents > 0)
            {
                loadInfo.m_pxmf3BiTangents = new XMFLOAT3[loadInfo.nBiTangents];
                nReads = (UINT)::fread(loadInfo.m_pxmf3BiTangents, sizeof(XMFLOAT3), loadInfo.nBiTangents, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<Normals>:")) {
            nNormals = ::ReadIntegerFromFile(pInFile);
            loadInfo.nNormals = nNormals;
            if (nNormals > 0) {
                loadInfo.m_nType |= VERTEXT_NORMAL;
                loadInfo.m_pxmf3Normals = new XMFLOAT3[nNormals];
                nReads = (UINT)::fread(loadInfo.m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<Indices>:")) {
            nIndices = ::ReadIntegerFromFile(pInFile);
            if (nIndices > 0) {
                loadInfo.m_pnIndices = new UINT[nIndices];
                nReads = (UINT)::fread(loadInfo.m_pnIndices, sizeof(int), nIndices, pInFile);
            }
        }
        else if (!strcmp(pstrToken, "<SubMeshes>:")) {
            loadInfo.m_nSubMeshes = ::ReadIntegerFromFile(pInFile);
            if (loadInfo.m_nSubMeshes > 0) {
                loadInfo.m_pnSubSetIndices = new int[loadInfo.m_nSubMeshes];
                loadInfo.m_ppnSubSetIndices = new UINT * [loadInfo.m_nSubMeshes];
                for (int i = 0; i < loadInfo.m_nSubMeshes; i++) {
                    loadInfo.m_ppnSubSetIndices[i] = NULL;
                    ::ReadStringFromFile(pInFile, pstrToken);
                    if (!strcmp(pstrToken, "<SubMesh>:")) {
                        int nIndex = ::ReadIntegerFromFile(pInFile);
                        loadInfo.m_pnSubSetIndices[i] = ::ReadIntegerFromFile(pInFile);
                        if (loadInfo.m_pnSubSetIndices[i] > 0) {
                            loadInfo.m_ppnSubSetIndices[i] = new UINT[loadInfo.m_pnSubSetIndices[i]];
                            nReads = (UINT)::fread(loadInfo.m_ppnSubSetIndices[i], sizeof(UINT), loadInfo.m_pnSubSetIndices[i], pInFile);
                        }
                    }
                }
            }
        }
        else if (!strcmp(pstrToken, "</Mesh>")) {
            break;
        }
    }
    *info = Li2i(loadInfo);

    for (int i = 0; i < loadInfo.m_nSubMeshes; i++) {
        delete loadInfo.m_ppnSubSetIndices[i];
    }

    delete loadInfo.m_pxmf3Normals;
    delete loadInfo.m_pxmf3Positions;
    delete loadInfo.m_pxmf4Colors;
    delete loadInfo.m_pnIndices;
    delete loadInfo.m_ppnSubSetIndices;
    delete loadInfo.m_pnSubSetIndices;
}

void CMeshLoader::LoadMaterial(FILE* pInFile, CMeshInfo* info) {
    char pstrToken[64] = { '\0' };
    UINT nReads = 0;

    int nMaterial = 0;
    float temp;

    CMeshMaterialInfo t;

    int m_nMaterials = ::ReadIntegerFromFile(pInFile);

    for (; ; ) {
        ::ReadStringFromFile(pInFile, pstrToken);

        if (!strcmp(pstrToken, "<Material>:")) {
            nMaterial = ::ReadIntegerFromFile(pInFile);
            info->materials.push_back(t);
        }
        else if (!strcmp(pstrToken, "<AlbedoColor>:")) {
            nReads = (UINT)::fread(&(info->materials[nMaterial].diffuse), sizeof(float), 4, pInFile);
        }
        else if (!strcmp(pstrToken, "<EmissiveColor>:")) {
            nReads = (UINT)::fread(&(info->materials[nMaterial].ambient), sizeof(float), 4, pInFile);
        }
        else if (!strcmp(pstrToken, "<SpecularColor>:")) {
            nReads = (UINT)::fread(&(info->materials[nMaterial].specular), sizeof(float), 4, pInFile);
        }
        else if (!strcmp(pstrToken, "<Glossiness>:")) {
            nReads = (UINT)::fread(&(temp), sizeof(float), 1, pInFile);
        }
        else if (!strcmp(pstrToken, "<Smoothness>:")) {
            nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
        }
        else if (!strcmp(pstrToken, "<Metallic>:")) {
            nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
        }
        else if (!strcmp(pstrToken, "<SpecularHighlight>:")) {
            nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
        }
        else if (!strcmp(pstrToken, "<GlossyReflection>:")) {
            nReads = (UINT)::fread(&(temp), sizeof(float), 1, pInFile);
        }
        else if (!strcmp(pstrToken, "<AlbedoMap>:"))
        {
            BYTE nStrLength = 64;
            UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
            info->materials[nMaterial].strlenD = nStrLength;
            nReads = (UINT)::fread(info->materials[nMaterial].diffuseTexName, sizeof(char), nStrLength, pInFile);
            info->materials[nMaterial].diffuseTexName[nStrLength] = '\0';
        }
        else if (!strcmp(pstrToken, "<SpecularMap>:"))
        {
            BYTE nStrLength = 64;
            UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
            info->materials[nMaterial].strlenS = nStrLength; 
            nReads = (UINT)::fread(info->materials[nMaterial].specularTexName, sizeof(char), nStrLength, pInFile);
            info->materials[nMaterial].specularTexName[nStrLength] = '\0';
        }
        else if (!strcmp(pstrToken, "<NormalMap>:"))
        {
            BYTE nStrLength = 64;
            UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
            info->materials[nMaterial].strlenN = nStrLength;
            nReads = (UINT)::fread(info->materials[nMaterial].normalTexName, sizeof(char), nStrLength, pInFile);
            info->materials[nMaterial].normalTexName[nStrLength] = '\0';
        }
        else if (!strcmp(pstrToken, "<MetallicMap>:"))
        {
            ::ReadStringFromFile(pInFile, pstrToken);
        }
        else if (!strcmp(pstrToken, "<EmissionMap>:"))
        {
            ::ReadStringFromFile(pInFile, pstrToken);
        }
        else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
        {
            BYTE nStrLength = 64;
            UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
            info->materials[nMaterial].strlenDD = nStrLength;
            nReads = (UINT)::fread(info->materials[nMaterial].detailDiffuseTexName, sizeof(char), nStrLength, pInFile);
            info->materials[nMaterial].detailDiffuseTexName[nStrLength] = '\0';
        }
        else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
        {
            BYTE nStrLength = 64;
            UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
            info->materials[nMaterial].strlenDN = nStrLength;
            nReads = (UINT)::fread(info->materials[nMaterial].detailNormalTexName, sizeof(char), nStrLength, pInFile);
            info->materials[nMaterial].detailNormalTexName[nStrLength] = '\0';
        }
        else if (!strcmp(pstrToken, "</Materials>")) {
            break;
        }
    }
}

CMeshInfo CMeshLoader::LoadFrameHierarchy(FILE* pInFile) {
    char pstrToken[64] = { '\0' };
    UINT nReads = 0;

    XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
    XMFLOAT4 xmf4Rotation;
    XMFLOAT4X4 matrix;

    CMeshInfo info;

    BYTE nStrLength = 0;
    int nFrame = 0, nTextures = 0;
    info.transform = make_shared<Transform>();

    while (1) {
        ::ReadStringFromFile(pInFile, pstrToken);
        if (!strcmp(pstrToken, "<Frame>:")) {
            nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
            nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

            ::ReadStringFromFile(pInFile, pstrToken);
            info.name = pstrToken;
        }
        else if (!strcmp(pstrToken, "<Transform>:")) {
            nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
            nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
            nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
            nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
        }
        else if (!strcmp(pstrToken, "<TransformMatrix>:")) {
            nReads = (UINT)::fread(&matrix, sizeof(float), 16, pInFile);
        }
        else if (!strcmp(pstrToken, "<Mesh>:")) {
            LoadMesh(pInFile, &info);
        }
        else if (!strcmp(pstrToken, "<Materials>:")) {
            LoadMaterial(pInFile, &info);
        }
        else if (!strcmp(pstrToken, "<Children>:")) {
            int nChilds = ::ReadIntegerFromFile(pInFile);

            if (nChilds > 0) {
                _childCount.push_back(nChilds);
                _parentCount.push_back(_meshes.size() - 1);
                for (int i = 0; i < nChilds; i++) {
                    int32 count = _count;
                    _count++;
                    CMeshInfo mesh;
                    _meshes.push_back(mesh);
                    _meshes.at(count) = move(LoadFrameHierarchy(pInFile));
                }
            }
        }
        else if (!strcmp(pstrToken, "</Frame>")) {
            break;
        }
    }
    
    if(!info.transform)
        info.transform = make_shared<Transform>();

    Vec3 position, scale, rotation;
    Quaternion quaternion;

    DecomposeMatrix(matrix, position, quaternion, scale);
    rotation = QuaternionToEuler(xmf4Rotation);

    info.transform->SetLocalPosition(xmf3Position);
    info.transform->SetLocalScale(xmf3Scale);
    info.transform->SetLocalRotation(xmf3Rotation);
    info.transform->SetLocalRotationQuaternion(xmf4Rotation);

    return info;
}


void CMeshLoader::LoadGeometry(const wstring& FileName)
{
    string path = ws2s(FileName);

    char* c = const_cast<char*>(path.c_str());
    c[path.size() + 1] = '\0';
    FILE* pInFile = NULL;
    ::fopen_s(&pInFile, c, "rb");

    assert(pInFile);
    ::rewind(pInFile);

    int32 count = _count;
    _count++;
    CMeshInfo mesh;
    _meshes.push_back(mesh);
    _meshes.at(count) = move(LoadFrameHierarchy(pInFile));

}

CMeshInfo CMeshLoader::Li2i(CMeshLoadInfo loadInfo) {
    CMeshInfo temp;
    temp.name = loadInfo.m_pstrMeshName;

    for (uint32 i = 0; i < loadInfo.nPositions; i++) {
        Vertex vertex;
        vertex.pos.x = loadInfo.m_pxmf3Positions[i].x;
        vertex.pos.y = loadInfo.m_pxmf3Positions[i].y;
        vertex.pos.z = loadInfo.m_pxmf3Positions[i].z;
        vertex.normal.x = (loadInfo.m_pxmf3Normals[i].x);
        vertex.normal.y = (loadInfo.m_pxmf3Normals[i].y);
        vertex.normal.z = (loadInfo.m_pxmf3Normals[i].z);
        vertex.tangent = loadInfo.m_pxmf3Tangents[i];
        vertex.uv = loadInfo.m_pxmf2TextureCoords0[i];
        //vertex.uv2 = loadInfo.m_pxmf2TextureCoords1[i];
        temp.vertices.push_back(vertex);
    }



    {
        if (loadInfo.m_nIndices != 0) {
            vector<uint32> idx;
            for (int i = 0; i < loadInfo.m_nIndices; i++) {
                idx.push_back(static_cast<uint32>(loadInfo.m_pnIndices[i]));
            }
            temp.indices.push_back(idx);

            //	for (uint32 i = 0; i < loadInfo.m_nVertices - (loadInfo.m_nSubMeshes * 3); i++) {
            //		idx.push_back(i);
            //	}
            //	temp.indices.push_back(idx);
        }
    }

    {
        vector<uint32> idx;
        for (int i = 0; i < loadInfo.m_nSubMeshes; i++) {
            for (int j = 0; j < loadInfo.m_pnSubSetIndices[i]; j++) {
                idx.push_back(static_cast<uint32>(loadInfo.m_ppnSubSetIndices[i][j]));
            }
        }
        temp.indices.push_back(idx);
    }

    return temp;
}

void CMeshLoader::CreateTextures() {
    for (size_t i = 0; i < _meshes.size(); i++) {
        for (size_t j = 0; j < _meshes[i].materials.size(); j++) {

            // DiffuseTexture
            {
                BYTE nStrLength = _meshes[i].materials[j].strlenD;
                bool bDuplicated = false;
                bool bLoaded = false;
                if (strcmp(_meshes[i].materials[j].diffuseTexName, "null"))
                {
                    bLoaded = true;
                    char pstrFilePath[254] = { '\0' };
                    strcpy_s(pstrFilePath, 64, "../Resources/BIN/Textures/");

                    bDuplicated = (_meshes[i].materials[j].diffuseTexName[0] == '@');
                    strcpy_s(pstrFilePath + 26, 64 - 26, (bDuplicated) ? (_meshes[i].materials[j].diffuseTexName + 1) : _meshes[i].materials[j].diffuseTexName);
                    strcpy_s(pstrFilePath + 26 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

                    string filename(_meshes[i].materials[j].diffuseTexName);
                    string fullPath(pstrFilePath);
                    GET_SINGLETON(Resources)->Load<Texture>(s2ws(filename), s2ws(fullPath));
                }
            }

            // NormalTexture
            {
                BYTE nStrLength = _meshes[i].materials[j].strlenN;
                bool bDuplicated = false;
                bool bLoaded = false;
                if (strcmp(_meshes[i].materials[j].normalTexName, "null"))
                {
                    bLoaded = true;
                    char pstrFilePath[254] = { '\0' };
                    strcpy_s(pstrFilePath, 64, "../Resource/BIN/Textures/");

                    bDuplicated = (_meshes[i].materials[j].normalTexName[0] == '@');
                    strcpy_s(pstrFilePath + 25, 64 - 25, (bDuplicated) ? (_meshes[i].materials[j].normalTexName + 1) : _meshes[i].materials[j].normalTexName);
                    strcpy_s(pstrFilePath + 25 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

                    string filename(_meshes[i].materials[j].normalTexName);
                    string fullPath(pstrFilePath);
                    GET_SINGLETON(Resources)->Load<Texture>(s2ws(filename), s2ws(fullPath));
                }
            }

            // SpecularTexture
            {
                BYTE nStrLength = _meshes[i].materials[j].strlenS;
                bool bDuplicated = false;
                bool bLoaded = false;
                if (strcmp(_meshes[i].materials[j].specularTexName, "null"))
                {
                    bLoaded = true;
                    char pstrFilePath[254] = { '\0' };
                    strcpy_s(pstrFilePath, 64, "../Resource/BIN/Textures/");

                    bDuplicated = (_meshes[i].materials[j].specularTexName[0] == '@');
                    strcpy_s(pstrFilePath + 25, 64 - 25, (bDuplicated) ? (_meshes[i].materials[j].specularTexName + 1) : _meshes[i].materials[j].specularTexName);
                    strcpy_s(pstrFilePath + 25 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

                    string filename(_meshes[i].materials[j].specularTexName);
                    string fullPath(pstrFilePath);
                    GET_SINGLETON(Resources)->Load<Texture>(s2ws(filename), s2ws(fullPath));
                }
            }
        }
    }
}

void CMeshLoader::CreateMaterials() {
    for (size_t i = 0; i < _meshes.size(); i++) {
        for (size_t j = 0; j < _meshes[i].materials.size(); j++) {
            shared_ptr<Material> material = make_shared<Material>();
            wstring key = _meshes[i].materials[j].name;
            material->SetName(key);
            material->SetShader(GET_SINGLETON(Resources)->Get<Shader>(L"Deferred"));

            {
                string filename(_meshes[i].materials[j].diffuseTexName);
                wstring key = s2ws(filename);
                shared_ptr<Texture> diffuseTexture = GET_SINGLETON(Resources)->Get<Texture>(key);
                if (diffuseTexture)
                    material->SetTexture(0, diffuseTexture);
            }

            {
                string filename(_meshes[i].materials[j].normalTexName);
                wstring key = s2ws(filename);
                shared_ptr<Texture> normalTexture = GET_SINGLETON(Resources)->Get<Texture>(key);
                if (normalTexture)
                    material->SetTexture(1, normalTexture);
            }

            {
                string filename(_meshes[i].materials[j].specularTexName);
                wstring key = s2ws(filename);
                shared_ptr<Texture> specularTexture = GET_SINGLETON(Resources)->Get<Texture>(key);
                if (specularTexture)
                    material->SetTexture(2, specularTexture);
            }

            GET_SINGLETON(Resources)->Add<Material>(material->GetName(), material);
        }
    }
}