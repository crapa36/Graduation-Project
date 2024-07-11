#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

// ���������� ���������� ������Ʈ�ϴ� �Լ��Դϴ�.
void Frustum::FinalUpdate() {

    // ī�޶��� �� ��Ʈ������ �������� ��Ʈ������ ������� ����մϴ�.
    Matrix matViewInv = Camera::S_MatView.Invert();
    Matrix matProjectionInv = Camera::S_MatProjection.Invert();

    // �� ������� ���Ͽ� ���� ��ȯ ��Ʈ������ ����ϴ�.
    Matrix matInv = matProjectionInv * matViewInv;

    // Ŭ�� ������ �𼭸��� ���� �������� ��ȯ�մϴ�.
    vector<Vec3> worldPos = {
        ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, 1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, -1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, 1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, -1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 1.f), matInv)
    };

    // ���� ������ ������ ����Ͽ� ���������� ���� �����մϴ�.
    _planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // �ð� ����
    _planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // �ݽð� ����
    _planes[PLANE_TOP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // �ð� ����
    _planes[PLANE_BOTTOM] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // �ݽð� ����
    _planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // �ð� ����
    _planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // �ݽð� ����
}

// �־��� ��ü�� �������� ���� ���ԵǴ��� ���θ� �Ǵ��ϴ� �Լ��Դϴ�.
bool Frustum::ContainsSphere(const Vec3& pos, float radius) {
    for (const Vec4& plane : _planes) {

        // ����� ���� ���͸� ���մϴ�.
        Vec3 normal = Vec3(plane.x, plane.y, plane.z);

        // ��ü�� �߽��� ������κ����� �Ÿ��� ���������� ũ�� �������� �ۿ� �ֽ��ϴ�.
        if (normal.Dot(pos) + plane.w > radius)
            return false;
    }

    return true;
}