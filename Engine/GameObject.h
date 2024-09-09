#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class MonoBehaviour;
class Camera;
class Light;
class ParticleSystem;
class Terrain;
class BaseCollider;
class Animator;
class Rigidbody;

class GameObject :public Object, public enable_shared_from_this<GameObject> {
public:
    GameObject();
    virtual ~GameObject();

    void Awake();
    void Start();
    void Update();
    void LateUpdate();
    void FinalUpdate();

    shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

    shared_ptr<Transform> GetTransform();
    shared_ptr<MeshRenderer> GetMeshRenderer();
    shared_ptr<Camera> GetCamera();
    shared_ptr<Light> GetLight();
    shared_ptr<ParticleSystem> GetParticleSystem();
    shared_ptr<Terrain> GetTerrain();
    shared_ptr<BaseCollider> GetCollider();
    shared_ptr<Animator> GetAnimator();
    shared_ptr<Rigidbody> GetRigidbody();

    wstring GetPath() { return _path; }

    void AddComponent(shared_ptr<Component> component);

    void SetCheckFrustum(bool checkFrustum) { _checkFrustum = checkFrustum; }
    bool GetCheckFrustum() const { return _checkFrustum; }

    void SetLayerIndex(uint8 layerIndex) { _layerIndex = layerIndex; }
    uint8 GetLayerIndex() { return _layerIndex; }

    void SetStatic(bool isStatic) { _isStatic = isStatic; }
    bool IsStatic() { return _isStatic; }

private:
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    vector<shared_ptr<MonoBehaviour>> _scripts;

    shared_ptr<GameObject>          _parentsObject;
    vector<shared_ptr<GameObject>> _childObject;

    wstring _path = L"";
    bool _checkFrustum = true;
    uint8 _layerIndex = 0;
    bool _isStatic = true;
};
