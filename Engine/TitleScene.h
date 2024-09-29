#pragma once
#include "Scene.h"
class TitleScene {
public:
    TitleScene();
    virtual ~TitleScene();

    shared_ptr<Scene> GetScene() { return _scene; }
private:
    shared_ptr<Scene> _scene = make_shared<Scene>();
};
