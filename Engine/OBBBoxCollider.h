#pragma once
#include "BaseCollider.h" // 기본 충돌 감지기 클래스 포함

// 객체 정렬 경계 상자를 나타내는 클래스
class OBBBoxCollider : public BaseCollider {
public:

    // 생성자
    OBBBoxCollider();

    // 소멸자
    virtual ~OBBBoxCollider();

    // 매 프레임마다 충돌 감지기 상태 업데이트
    virtual void Update() override;

    // 레이와 충돌 감지 여부 판단
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;

    // 다른 충돌 감지기와의 교차 여부 판단
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    // 경계 상자 가져오기
    BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }

private:
    BoundingOrientedBox _boundingBox; // 경계 상자
};