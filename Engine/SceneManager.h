#pragma once

class Scene;

enum {
    MAX_LAYER = 32
};

class SceneManager {
    DECLARE_SINGLETON(SceneManager);

public:
    void Init();
    void Update();
    void Render();

    void SaveScene(wstring sceneName);
    void LoadScene(wstring sceneName);
    void ChangeScene(wstring sceneName);

    void SetLayerName(uint8 index, const wstring& name);
    const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
    uint8 LayerNameToIndex(const wstring& name);

public:
    shared_ptr<Scene> GetActiveScene() { return _activeScene; }
    shared_ptr<Scene> GetScene() { return _activeScene; }

private:
    map<wstring, shared_ptr<Scene>> Scenes;
    shared_ptr<Scene> _beforeScene;
    shared_ptr<Scene> _activeScene;

    array<wstring, MAX_LAYER> _layerNames;
    map<wstring, uint8> _layerIndex;
};
