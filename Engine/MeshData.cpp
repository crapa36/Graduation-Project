#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "CMeshLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA) {
}

MeshData::~MeshData() {
}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring& path) {
    FBXLoader loader;
    loader.LoadFbx(path);

    shared_ptr<MeshData> meshData = make_shared<MeshData>();

    for (int32 i = 0; i < loader.GetMeshCount(); i++) {
        shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i), loader);

        GET_SINGLETON(Resources)->Add<Mesh>(mesh->GetName(), mesh);

        // Material 찾아서 연동
        vector<shared_ptr<Material>> materials;
        for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++) {
            shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
            materials.push_back(material);
        }

        MeshRenderInfo info = {};
        info.mesh = mesh;
        info.materials = materials;
        meshData->_meshRenders.push_back(info);
    }

    return meshData;
}

shared_ptr<MeshData> MeshData::LoadFromBIN(const wstring& path) {
    CMeshLoader loader;
    loader.LoadBIN(path);

    shared_ptr<MeshData> meshData = make_shared<MeshData>();

    for (int32 i = 0; i < loader.GetMeshCount(); i++) {
        if (!loader.GetMesh(i).vertices.empty()) {
            shared_ptr<Mesh> mesh = Mesh::CreateFromBIN(&loader.GetMesh(i), loader);
            mesh->SetTransform(loader.GetMesh(i).transform);
            //mesh->SetTransform(make_shared<Transform>());
            GET_SINGLETON(Resources)->Add<Mesh>(mesh->GetName(), mesh);

            // Material 찾아서 연동
            vector<shared_ptr<Material>> materials;
            for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++) {
                shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
                materials.push_back(material);
            }

            MeshRenderInfo info = {};
            info.mesh = mesh;
            info.materials = materials;
            meshData->_meshRenders.push_back(info);
        }
    }
    return meshData;
}

void MeshData::Load(const wstring& _strFilePath) {

    // TODO
}

void MeshData::Save(const wstring& _strFilePath) {

    // TODO
}

vector<shared_ptr<GameObject>> MeshData::Instantiate() {
    vector<shared_ptr<GameObject>> v;

    for (MeshRenderInfo& info : _meshRenders) {
        shared_ptr<GameObject> gameObject = make_shared<GameObject>();
        gameObject->AddComponent(make_shared<Transform>());
        //gameObject->AddComponent(info.mesh->GetTransform());
        gameObject->AddComponent(make_shared<MeshRenderer>());
        gameObject->GetMeshRenderer()->SetMesh(info.mesh);

        for (uint32 i = 0; i < info.materials.size(); i++)
            gameObject->GetMeshRenderer()->SetMaterial(info.materials[i], i);

        if (info.mesh->IsAnimMesh()) {
            shared_ptr<Animator> animator = make_shared<Animator>();
            gameObject->AddComponent(animator);
            animator->SetBones(info.mesh->GetBones());
            animator->SetAnimClip(info.mesh->GetAnimClip());
        }

        v.push_back(gameObject);
    }

    return v;
}