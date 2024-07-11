#pragma once

#include <array>
#include <cstdint>
using namespace std;

// 뷰 프러스텀의 각 면을 나타내는 열거형입니다.
enum PLANE_TYPE : uint8_t {
    PLANE_FRONT,
    PLANE_BACK,
    PLANE_TOP,
    PLANE_BOTTOM,
    PLANE_LEFT,
    PLANE_RIGHT,
    PLANE_END
};

// 프러스텀을 나타내는 클래스입니다. 카메라의 시야를 결정하는데 사용됩니다.
class Frustum {
public:

    // 프러스텀을 최종적으로 업데이트합니다. 카메라의 뷰와 프로젝션 매트릭스를 기반으로 계산됩니다.
    void FinalUpdate();

    // 주어진 구체가 프러스텀 내에 포함되는지 여부를 판단합니다.
    bool ContainsSphere(const Vec3& pos, float radius);

private:

    // 프러스텀의 각 면을 나타내는 평면의 배열입니다.
    array<Vec4, PLANE_END> _planes;
};
