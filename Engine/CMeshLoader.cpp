#include "pch.h"
#include "CMeshLoader.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Shader.h"
#include "Material.h"
#include "Transform.h"

/// 

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}
///
CMeshLoader::CMeshLoader()
{
}

CMeshLoader::~CMeshLoader()
{
}

void CMeshLoader::LoadBIN(const wstring& path)
{
	LoadGeometry(path);

    // 快府 备炼俊 嘎霸 Texture / Material 积己
    //CreateTextures();
    CreateMaterials();
}

void CMeshLoader::LoadMesh(FILE *pInFile, CMeshInfo* info)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	CMeshLoadInfo loadInfo;

	loadInfo.m_nVertices = ::ReadIntegerFromFile(pInFile);
	::ReadStringFromFile(pInFile, loadInfo.m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(loadInfo.m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&(loadInfo.m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			loadInfo.nPositions = ::ReadIntegerFromFile(pInFile);
			
			if (loadInfo.nPositions > 0)
			{
				loadInfo.m_pxmf3Positions = new XMFLOAT3[loadInfo.nPositions];
				nReads = (UINT)::fread(loadInfo.m_pxmf3Positions, sizeof(XMFLOAT3), loadInfo.nPositions, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(pInFile);
			loadInfo.nColors = nColors;
			if (nColors > 0)
			{
				loadInfo.m_nType |= VERTEXT_COLOR;
				loadInfo.m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(loadInfo.m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(pInFile);
			loadInfo.nNormals = nNormals;
			if (nNormals > 0)
			{
				loadInfo.m_nType |= VERTEXT_NORMAL;
				loadInfo.m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(loadInfo.m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			nIndices = ::ReadIntegerFromFile(pInFile);
			if (nIndices > 0)
			{
				loadInfo.m_pnIndices = new UINT[nIndices];
				nReads = (UINT)::fread(loadInfo.m_pnIndices, sizeof(int), nIndices, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			loadInfo.m_nSubMeshes = ::ReadIntegerFromFile(pInFile);
			if (loadInfo.m_nSubMeshes > 0)
			{
				loadInfo.m_pnSubSetIndices = new int[loadInfo.m_nSubMeshes];
				loadInfo.m_ppnSubSetIndices = new UINT * [loadInfo.m_nSubMeshes];
				for (int i = 0; i < loadInfo.m_nSubMeshes; i++)
				{
					loadInfo.m_ppnSubSetIndices[i] = NULL;
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = ::ReadIntegerFromFile(pInFile);
						loadInfo.m_pnSubSetIndices[i] = ::ReadIntegerFromFile(pInFile);
						if (loadInfo.m_pnSubSetIndices[i] > 0)
						{
							loadInfo.m_ppnSubSetIndices[i] = new UINT[loadInfo.m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(loadInfo.m_ppnSubSetIndices[i], sizeof(UINT), loadInfo.m_pnSubSetIndices[i], pInFile);
						}

					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
	*info = Li2i(loadInfo);
}

void CMeshLoader::LoadMaterial(FILE* pInFile, CMeshInfo* info)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nMaterial = 0;
	float temp;

	CMeshMaterialInfo t;

	int m_nMaterials = ::ReadIntegerFromFile(pInFile);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ::ReadIntegerFromFile(pInFile);
			info->materials.push_back(t);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(info->materials[nMaterial].diffuse), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(info->materials[nMaterial].ambient), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(info->materials[nMaterial].specular), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(temp), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(temp), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}


CMeshInfo CMeshLoader::LoadFrameHierarchy(FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;
	
	CMeshInfo info;

	int nFrame = 0;

	while (1)
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{

			nFrame = ::ReadIntegerFromFile(pInFile);
			//_meshes.resize(nFrame);
		
			::ReadStringFromFile(pInFile, pstrToken);
			info.name = pstrToken;
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion

			info._transform.SetLocalPosition(xmf3Position);
			info._transform.SetLocalRotation(xmf3Rotation);
			info._transform.SetLocalScale(xmf3Scale);
			info._transform.SetLocalRotationQuaternion(xmf4Rotation);
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(pstrToken, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			LoadMesh(pInFile, &info);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			LoadMaterial(pInFile, &info);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					_meshes.push_back(LoadFrameHierarchy(pInFile));
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}

	return info;
}


void CMeshLoader::LoadGeometry(const wstring& FileName)
{
	string path = ws2s(FileName);
	
	char* c = const_cast<char*>(path.c_str());
	c[path.size() + 1] = '\0';
	FILE* pInFile = NULL;
	cout << c;
	::fopen_s(&pInFile, c, "rb");

	assert(pInFile);
	::rewind(pInFile);

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Hierarchy>:"))
		{
			_meshes.push_back(LoadFrameHierarchy(pInFile));
		}
		else if (!strcmp(pstrToken, "</Hierarchy>"))
		{
			break;
		}
	}
}


CMeshInfo CMeshLoader::Li2i(CMeshLoadInfo loadInfo)
{
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
			vertex.tangent = Vec3(1.0f, 0.0f, 0.0f);
			temp.vertices.push_back(vertex);
		}

		for (int i = 0; i < loadInfo.m_nIndices; i += 3) {
			vector<uint32> idx;
			idx.push_back(static_cast<uint32>(loadInfo.m_pnIndices[i]));
			idx.push_back(static_cast<uint32>(loadInfo.m_pnIndices[i + 1]));
			idx.push_back(static_cast<uint32>(loadInfo.m_pnIndices[i + 2]));
			temp.indices.push_back(idx);
		}
		
		if (loadInfo.m_nIndices == 0) {
			for (uint32 i = 0; i < loadInfo.m_nVertices - (loadInfo.m_nSubMeshes * 3); i += 3) {
				vector<uint32> idx;
				idx.push_back(i);
				idx.push_back(i + 1);
				idx.push_back(i + 2);
				temp.indices.push_back(idx);
			}
		}

		for (int i = 0; i < loadInfo.m_nSubMeshes * 3 - 1; i+= 3) {
			vector<uint32> idx;
			idx.push_back(static_cast<uint32>(loadInfo.m_pnSubSetIndices[i]));
			idx.push_back(static_cast<uint32>(loadInfo.m_pnSubSetIndices[i + 1]));
			idx.push_back(static_cast<uint32>(loadInfo.m_pnSubSetIndices[i + 2]));
			temp.indices.push_back(idx);
		}

		return temp;
}

void CMeshLoader::CreateTextures() {
    for (size_t i = 0; i < _meshes.size(); i++) {
        for (size_t j = 0; j < _meshes[i].materials.size(); j++) {

            // DiffuseTexture
            {
                wstring relativePath = _meshes[i].materials[j].diffuseTexName.c_str();
                wstring filename = fs::path(relativePath).filename();
                wstring fullPath = _resourceDirectory + L"\\" + filename;
                if (filename.empty() == false)
                    GET_SINGLETON(Resources)->Load<Texture>(filename, fullPath);
            }

            // NormalTexture
            {
                wstring relativePath = _meshes[i].materials[j].normalTexName.c_str();
                wstring filename = fs::path(relativePath).filename();
                wstring fullPath = _resourceDirectory + L"\\" + filename;
                if (filename.empty() == false)
                    GET_SINGLETON(Resources)->Load<Texture>(filename, fullPath);
            }

            // SpecularTexture
            {
                wstring relativePath = _meshes[i].materials[j].specularTexName.c_str();
                wstring filename = fs::path(relativePath).filename();
                wstring fullPath = _resourceDirectory + L"\\" + filename;
                if (filename.empty() == false)
                    GET_SINGLETON(Resources)->Load<Texture>(filename, fullPath);
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
                wstring diffuseName = _meshes[i].materials[j].diffuseTexName.c_str();
                wstring filename = fs::path(diffuseName).filename();
                wstring key = filename;
                shared_ptr<Texture> diffuseTexture = GET_SINGLETON(Resources)->Get<Texture>(key);
                if (diffuseTexture)
                    material->SetTexture(0, diffuseTexture);
            }

            {
                wstring normalName = _meshes[i].materials[j].normalTexName.c_str();
                wstring filename = fs::path(normalName).filename();
                wstring key = filename;
                shared_ptr<Texture> normalTexture = GET_SINGLETON(Resources)->Get<Texture>(key);
                if (normalTexture)
                    material->SetTexture(1, normalTexture);
            }

            {
                wstring specularName = _meshes[i].materials[j].specularTexName.c_str();
                wstring filename = fs::path(specularName).filename();
                wstring key = filename;
                shared_ptr<Texture> specularTexture = GET_SINGLETON(Resources)->Get<Texture>(key);
                if (specularTexture)
                    material->SetTexture(2, specularTexture);
            }

            GET_SINGLETON(Resources)->Add<Material>(material->GetName(), material);
        }
    }
}
