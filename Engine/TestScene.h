#pragma once
#include "Scene.h"
class TestScene {
public:
    TestScene();
    virtual ~TestScene();

    shared_ptr<Scene> GetScene() { return _scene; }
private:
    shared_ptr<Scene> _scene = make_shared<Scene>();
};
