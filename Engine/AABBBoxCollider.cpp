#include "pch.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"
#include "OBBBoxCollider.h"

// ������: �࿡ ���ĵ� ��� ����(AABB) �浹 ������� �ʱ�ȭ�մϴ�.
AABBBoxCollider::AABBBoxCollider() : BaseCollider(ColliderType::AABB) {

    // �ʱ�ȭ �ڵ带 ���⿡ �߰��� �� �ֽ��ϴ�.
}

// �Ҹ���: ��ü�� �ı��Ǳ� ���� ���� �۾��� ó���մϴ�.
AABBBoxCollider::~AABBBoxCollider() {

    // ���� �ڵ带 ���⿡ �߰��� �� �ֽ��ϴ�.
}

// ������Ʈ: �浹 ������ ���� �浹 �����⸦ �غ��մϴ�. �Ϲ������� �����Ӹ��� �� ���� ȣ��˴ϴ�.
void AABBBoxCollider::Update() {

    // ��ü�� ���� ���¿� ����Ͽ� ��� ������ ũ�� �Ǵ� ��ġ�� ������Ʈ�մϴ�.
}

// ���� (Ray): ���̰� �� �浹 ������� �����ϴ��� ���θ� �����մϴ�.
bool AABBBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {

    // BoundingBox Ŭ������ Intersects �޼��带 ����Ͽ� ���̿��� ������ Ȯ���մϴ�.
    // ������ �߻��ϸ� 'distance' �Ű������� ���� ���������� �Ÿ��� ����˴ϴ�.
    return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

// ���� (Collider): �� �浹 �����Ⱑ �ٸ� �浹 ������� �����ϴ��� ���θ� �����մϴ�.
bool AABBBoxCollider::Intersects(const shared_ptr<BaseCollider>& other) {

    // �ٸ� �浹 �������� Ÿ���� Ȯ���ϰ� ������ �޼��带 ȣ���Ͽ� ���� �˻縦 �����մϴ�.
    ColliderType type = other->GetColliderType();

    switch (type) {
    case ColliderType::Sphere:

        // �ٸ� �浹 �����Ⱑ Sphere�� ���, ������ ������ Ȯ���մϴ�.
        return _boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
    case ColliderType::AABB:

        // �ٸ� �浹 �����Ⱑ ���� AABB�� ���, �ٸ� AABB���� ������ Ȯ���մϴ�.
        return _boundingBox.Intersects(dynamic_pointer_cast<AABBBoxCollider>(other)->GetBoundingBox());
    case ColliderType::OBB:

        // �ٸ� �浹 �����Ⱑ OBB(������ �ִ� ��� ����)�� ���, OBB���� ������ Ȯ���մϴ�.
        return _boundingBox.Intersects(dynamic_pointer_cast<OBBBoxCollider>(other)->GetBoundingBox());
    default:
        return false;
    }
}