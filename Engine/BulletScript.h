#pragma once
#include "MonoBehaviour.h"



class BulletScript : public MonoBehaviour {
public:
    BulletScript();
    virtual ~BulletScript();

    virtual void LateUpdate() override;

private:
    float _speed = 100.f;
};
