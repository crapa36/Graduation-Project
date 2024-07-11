#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

// 프러스텀을 최종적으로 업데이트하는 함수입니다.
void Frustum::FinalUpdate() {

    // 카메라의 뷰 매트릭스와 프로젝션 매트릭스의 역행렬을 계산합니다.
    Matrix matViewInv = Camera::S_MatView.Invert();
    Matrix matProjectionInv = Camera::S_MatProjection.Invert();

    // 두 역행렬을 곱하여 최종 변환 매트릭스를 얻습니다.
    Matrix matInv = matProjectionInv * matViewInv;

    // 클립 공간의 모서리를 월드 공간으로 변환합니다.
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

    // 월드 공간의 점들을 사용하여 프러스텀의 면을 정의합니다.
    _planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // 시계 방향
    _planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // 반시계 방향
    _planes[PLANE_TOP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // 시계 방향
    _planes[PLANE_BOTTOM] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // 반시계 방향
    _planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // 시계 방향
    _planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // 반시계 방향
}

// 주어진 구체가 프러스텀 내에 포함되는지 여부를 판단하는 함수입니다.
bool Frustum::ContainsSphere(const Vec3& pos, float radius) {
    for (const Vec4& plane : _planes) {

        // 평면의 법선 벡터를 구합니다.
        Vec3 normal = Vec3(plane.x, plane.y, plane.z);

        // 구체의 중심이 평면으로부터의 거리가 반지름보다 크면 프러스텀 밖에 있습니다.
        if (normal.Dot(pos) + plane.w > radius)
            return false;
    }

    return true;
}