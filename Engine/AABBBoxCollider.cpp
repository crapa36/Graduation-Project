#include "pch.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"
#include "OBBBoxCollider.h"

// 생성자: 축에 정렬된 경계 상자(AABB) 충돌 감지기로 초기화합니다.
AABBBoxCollider::AABBBoxCollider() : BaseCollider(ColliderType::AABB) {

    // 초기화 코드를 여기에 추가할 수 있습니다.
}

// 소멸자: 객체가 파괴되기 전에 정리 작업을 처리합니다.
AABBBoxCollider::~AABBBoxCollider() {

    // 정리 코드를 여기에 추가할 수 있습니다.
}

// 업데이트: 충돌 감지를 위해 충돌 감지기를 준비합니다. 일반적으로 프레임마다 한 번씩 호출됩니다.
void AABBBoxCollider::Update() {

    // 객체의 현재 상태에 기반하여 경계 상자의 크기 또는 위치를 업데이트합니다.
}

// 교차 (Ray): 레이가 이 충돌 감지기와 교차하는지 여부를 결정합니다.
bool AABBBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {

    // BoundingBox 클래스의 Intersects 메서드를 사용하여 레이와의 교차를 확인합니다.
    // 교차가 발생하면 'distance' 매개변수에 교차 지점까지의 거리가 저장됩니다.
    return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

// 교차 (Collider): 이 충돌 감지기가 다른 충돌 감지기와 교차하는지 여부를 결정합니다.
bool AABBBoxCollider::Intersects(const shared_ptr<BaseCollider>& other) {

    // 다른 충돌 감지기의 타입을 확인하고 적절한 메서드를 호출하여 교차 검사를 수행합니다.
    ColliderType type = other->GetColliderType();

    switch (type) {
    case ColliderType::Sphere:

        // 다른 충돌 감지기가 Sphere인 경우, 구와의 교차를 확인합니다.
        return _boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
    case ColliderType::AABB:

        // 다른 충돌 감지기가 또한 AABB인 경우, 다른 AABB와의 교차를 확인합니다.
        return _boundingBox.Intersects(dynamic_pointer_cast<AABBBoxCollider>(other)->GetBoundingBox());
    case ColliderType::OBB:

        // 다른 충돌 감지기가 OBB(방향이 있는 경계 상자)인 경우, OBB와의 교차를 확인합니다.
        return _boundingBox.Intersects(dynamic_pointer_cast<OBBBoxCollider>(other)->GetBoundingBox());
    default:
        return false;
    }
}