#pragma once
#include "MonoBehaviour.h"



class BulletScript : public MonoBehaviour {
public:
    BulletScript();
    virtual ~BulletScript();

    virtual void LateUpdate() override;

private:
    float _lifeTime = 10.f;
};
