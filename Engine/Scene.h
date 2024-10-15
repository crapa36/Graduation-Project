#pragma once

class GameObject;

class Scene {
public:
    void Awake();
    void Start();
    void Update();
    void LateUpdate();
    void FinalUpdate();

    void Render();

    void ClearRTV();

    void RenderShadow();
    void RenderDeferred();
    void RenderReflection();
    void RenderLights();
    void RenderFinal();

    void RenderForward();

    shared_ptr<class Camera> GetMainCamera();
    shared_ptr<class Camera> GetReflectionCamera();

private:
    void PushLightData();

public:
    void AddGameObject(shared_ptr<GameObject> gameObject);
    void RemoveGameObject(shared_ptr<GameObject> gameObject);

    const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

    void SaveScene(wstring path);
    void LoadScene(wstring path);

private:
    vector<shared_ptr<GameObject>>		_gameObjects;
    vector<shared_ptr<class Camera>>	_cameras;
    vector<shared_ptr<class Light>>		_lights;
};
