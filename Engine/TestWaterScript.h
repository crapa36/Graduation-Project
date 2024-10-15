#pragma once

#include "MonoBehaviour.h"

class TestWaterScript : public MonoBehaviour
{
public:
    TestWaterScript();
    virtual ~TestWaterScript();

    virtual void LateUpdate() override;

private:

};