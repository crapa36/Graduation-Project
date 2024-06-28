#pragma once

class Scene;

class SceneManager {
    DECLARE_SINGLETON(SceneManager);
public:
    void Update();
    void Render();
    void LoadScene(wstring sceneName);

    shared_ptr<Scene> GetActiveScene() { return _activeScene; }
private:
    shared_ptr<Scene> LoadTestScene();
    shared_ptr<Scene> _activeScene;
};
