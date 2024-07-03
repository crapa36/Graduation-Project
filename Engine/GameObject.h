#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class MonoBehaviour;
class Camera;
class Light;

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

    void AddComponent(shared_ptr<Component> component);

    void SetCheckFrustum(bool checkFrustum) { _checkFrustum = checkFrustum; }
    bool GetCheckFrustum() const { return _checkFrustum; }

    void SetLayerIndex(uint8 layerIndex) { _layerIndex = layerIndex; }
    uint8 GetLayerIndex() { return _layerIndex; }

private:
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    vector<shared_ptr<MonoBehaviour>> _scripts;

    bool _checkFrustum = true;
    uint8 _layerIndex = 0;
};