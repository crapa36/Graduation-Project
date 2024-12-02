#pragma once
#include "Component.h"

class Material;
class Mesh;

class BillboardRenderer : public Component {
public:
    BillboardRenderer();
    virtual ~BillboardRenderer();

    void FinalUpdate();
    void Render();

    void SetTexture(const wstring& texturePath);
    void SetAxisConstraint(const Vec3& axis); // 특정 축 기준 회전 제한

private:
    shared_ptr<Material> _material;
    shared_ptr<Mesh> _mesh;

    Vec3 _axisConstraint = { 0.0f, 1.0f, 0.0f }; // 축 제한 (0, 0, 0은 무제한)
};
