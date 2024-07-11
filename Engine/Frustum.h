#pragma once

#include <array>
#include <cstdint>
using namespace std;

// �� ���������� �� ���� ��Ÿ���� �������Դϴ�.
enum PLANE_TYPE : uint8_t {
    PLANE_FRONT,
    PLANE_BACK,
    PLANE_TOP,
    PLANE_BOTTOM,
    PLANE_LEFT,
    PLANE_RIGHT,
    PLANE_END
};

// ���������� ��Ÿ���� Ŭ�����Դϴ�. ī�޶��� �þ߸� �����ϴµ� ���˴ϴ�.
class Frustum {
public:

    // ���������� ���������� ������Ʈ�մϴ�. ī�޶��� ��� �������� ��Ʈ������ ������� ���˴ϴ�.
    void FinalUpdate();

    // �־��� ��ü�� �������� ���� ���ԵǴ��� ���θ� �Ǵ��մϴ�.
    bool ContainsSphere(const Vec3& pos, float radius);

private:

    // ���������� �� ���� ��Ÿ���� ����� �迭�Դϴ�.
    array<Vec4, PLANE_END> _planes;
};
