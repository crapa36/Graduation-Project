#pragma once
#include "MonoBehaviour.h"

class TestDragonScript : public MonoBehaviour {
public:
    virtual void Update() override;
    virtual void LateUpdate() override;

private:

    void ShotBullet();
    void MakeParticle();
    void SetbulletStartPos();
    vector<shared_ptr<GameObject>> _bullets;
    shared_ptr<GameObject> _particle;

    Vec3 _bulletStartPos = {};
};