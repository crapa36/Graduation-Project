#include "pch.h" // 프리 컴파일 헤더
#include "OBBBoxCollider.h" // OBBBoxCollider 클래스 선언 헤더
#include "SphereCollider.h" // SphereCollider 클래스 선언 헤더
#include "AABBBoxCollider.h" // AABBBoxCollider 클래스 선언 헤더

// 기본 생성자, ColliderType을 OBB로 설정
OBBBoxCollider::OBBBoxCollider() : BaseCollider(ColliderType::OBB) {

    // 여기에 초기화 코드 작성
}

// 소멸자
OBBBoxCollider::~OBBBoxCollider() {

    // 필요한 경우 리소스 해제 등의 정리 작업 수행
}

// 매 프레임마다 호출되어 객체 상태를 업데이트하는 메소드
void OBBBoxCollider::Update() {

    // 여기에서 OBB 상태를 업데이트하는 로직 구현
    // 예: 변환 행렬을 사용하여 위치, 회전, 크기 변경 반영
}

// 레이와의 충돌 검출
bool OBBBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {

    // _boundingBox를 사용하여 레이와의 충돌을 검출하고 결과를 반환
    return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

// 다른 충돌 감지기와의 충돌 검출
bool OBBBoxCollider::Intersects(const shared_ptr<BaseCollider>& other) {

    // 다른 충돌 감지기의 타입을 확인
    ColliderType type = other->GetColliderType();

    // 타입에 따라 적절한 충돌 검출 메소드 호출
    switch (type) {
    case ColliderType::Sphere: // 구와의 충돌
        return _boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
    case ColliderType::AABB: // AABB와의 충돌
        return _boundingBox.Intersects(dynamic_pointer_cast<AABBBoxCollider>(other)->GetBoundingBox());
    case ColliderType::OBB: // 다른 OBB와의 충돌
        return _boundingBox.Intersects(dynamic_pointer_cast<OBBBoxCollider>(other)->GetBoundingBox());
    }

    return false; // 지원되지 않는 충돌 감지기 타입의 경우 false 반환
}