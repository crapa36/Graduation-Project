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

    void AvoidObstaclesWithRays();

private:
    float		_speed = 100.f;

    bool        _isMouseLock = true;
    POINT       _centerPos;
    POINT       _centerScreenPos;

    Vec3 _dir;
};
