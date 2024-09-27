#pragma once

enum PLANE_TYPE : uint8 {
    PLANE_FRONT,
    PLANE_BACK,
    PLANE_TOP,
    PLANE_BOTTOM,
    PLANE_LEFT,
    PLANE_RIGHT,
    PLANE_END
};

//BoundingFrustum���� ��ü �ʿ�

class Frustum {
public:
    void FinalUpdate();
    bool ContainsSphere(const Vec3& pos, float radius);

    vector<Vec3> GetWorldPos();

private:
    array<Vec4, PLANE_END> _planes;
};
