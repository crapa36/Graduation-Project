#pragma once
#include "Component.h"

enum class LIGHT_TYPE : uint8 {
    DIRECTIONAL,
    POINT,
    SPOT,
};

struct LightColor {
    Vec4	diffuse;
    Vec4	ambient;
    Vec4	specular;
};

struct LightInfo {
    LightColor	color;
    Vec4		position;
    Vec4		direction;
    int32		lightType;
    float		range;
    float		angle;
    int32		padding;
};

struct LightParams {
    uint32		lightCount;
    Vec3		padding;
    LightInfo	lights[50];
};

class Light : public Component {
public:
    Light();
    virtual ~Light();

    virtual void FinalUpdate() override;
    void Render();
    void RenderShadow();

public:
    LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); }

    const LightInfo& GetLightInfo() { return _lightInfo; }
    shared_ptr<GameObject> GetShadowCamera() { return _shadowCamera; }

    void SetLightDirection(Vec3 direction);

    void SetDiffuse(const Vec3& diffuse) { _lightInfo.color.diffuse = diffuse; }
    void SetAmbient(const Vec3& ambient) { _lightInfo.color.ambient = ambient; }
    void SetSpecular(const Vec3& specular) { _lightInfo.color.specular = specular; }

    void SetLightType(LIGHT_TYPE type);
    void SetLightRange(float range) { _lightInfo.range = range; }
    void SetLightAngle(float angle) { _lightInfo.angle = angle; }

    void SetLightIndex(int8 index) { _lightIndex = index; }

    const Vec3& GetLightDirection();

    const Vec4& GetDiffuse() { return _lightInfo.color.diffuse; }
    const Vec4& GetAmbient() { return _lightInfo.color.ambient; }
    const Vec4& GetSpecular() { return _lightInfo.color.specular; }

    float GetLightRange() { return _lightInfo.range; }
    float GetLightAngle() { return _lightInfo.angle; }

    int8 GetLightIndex() { return _lightIndex; }

private:

    LightInfo _lightInfo = {};

    int8 _lightIndex = -1;
    shared_ptr<class Mesh> _volumeMesh;
    shared_ptr<class Material> _lightMaterial;

    shared_ptr<GameObject> _shadowCamera;
};
