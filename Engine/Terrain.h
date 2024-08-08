#pragma once
#include "Component.h"

class Terrain : public Component {
public:
    Terrain();
    virtual ~Terrain();

    void Init(int32 sizeX, int32 sizeZ);

    int32 GetsizeX() { return _sizeX; }
    int32 GetsizeZ() { return _sizeZ; }

    virtual void FinalUpdate() override;

private:
    int32 _sizeX = 15;
    int32 _sizeZ = 15;
    float _maxTesselation = 5.f;

    shared_ptr<class Material> _material;
};
