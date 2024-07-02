#pragma once
#include "Component.h"
#include "Frustum.h"

enum class PROJECTION_TYPE {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera : public Component {
public:
    Camera();
    virtual ~Camera();

    virtual void FinalUpdate() override;
    void Render();

    void SetprojectionType(PROJECTION_TYPE projectionType) { _projectionType = projectionType; }
    PROJECTION_TYPE GetProjectionType() { return _projectionType; }

    void SetCullingMaskLayerOnOff(uint8 layer, bool isOn) {
        if (isOn)
            _cullingMask |= 1 << layer;
        else
            _cullingMask &= ~(1 << layer);
    }
    void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
    void SetCullingMask(uint32 mask) { _cullingMask = mask; }
    bool IsCulled(uint8 layer) { return (_cullingMask & (1 << layer)) != 0; }

private:
    PROJECTION_TYPE _projectionType = PROJECTION_TYPE::PERSPECTIVE;

    float _near = 1.f;
    float _far = 1000.f;
    float _fov = XM_PI / 4.f;
    float _scale = 1.f;

    Matrix _matView = {};
    Matrix _matProjection = {};

    Frustum _frustum;
    uint32 _cullingMask = 0;

public:

    //TEMP
    static Matrix S_MatView;
    static Matrix S_MatProjection;
};
