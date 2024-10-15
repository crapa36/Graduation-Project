#pragma once
#include "MonoBehaviour.h"

class TestDragonScript : public MonoBehaviour {
public:
    virtual void Update() override;
    virtual void LateUpdate() override;


private:

    POINT       _centerPos;
    POINT       _centerScreenPos;
};