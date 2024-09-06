#pragma once
#include "MonoBehaviour.h"

enum Dir {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
};

class TestCameraScript : public MonoBehaviour {
public:
    TestCameraScript();
    virtual ~TestCameraScript();

    virtual void LateUpdate() override;

private:
    float		_speed = 100.f;

    bool        _isMouseLock = true;
    POINT       _centerPos;
    POINT       _centerScreenPos;

};
    