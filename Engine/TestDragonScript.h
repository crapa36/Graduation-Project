#pragma once
#include "MonoBehaviour.h"

class TestDragonScript : public MonoBehaviour {
public:
    virtual void Update() override;
    virtual void LateUpdate() override;

    void MakeBullet();


private:

    POINT       _centerPos;
    POINT       _centerScreenPos;
};