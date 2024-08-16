#include "pch.h" // ���� ������ ���
#include "OBBBoxCollider.h" // OBBBoxCollider Ŭ���� ���� ���
#include "SphereCollider.h" // SphereCollider Ŭ���� ���� ���
#include "AABBBoxCollider.h" // AABBBoxCollider Ŭ���� ���� ���

// �⺻ ������, ColliderType�� OBB�� ����
OBBBoxCollider::OBBBoxCollider() : BaseCollider(ColliderType::OBB) {

    // ���⿡ �ʱ�ȭ �ڵ� �ۼ�
}

// �Ҹ���
OBBBoxCollider::~OBBBoxCollider() {

    // �ʿ��� ��� ���ҽ� ���� ���� ���� �۾� ����
}

// �� �����Ӹ��� ȣ��Ǿ� ��ü ���¸� ������Ʈ�ϴ� �޼ҵ�
void OBBBoxCollider::Update() {

    // ���⿡�� OBB ���¸� ������Ʈ�ϴ� ���� ����
    // ��: ��ȯ ����� ����Ͽ� ��ġ, ȸ��, ũ�� ���� �ݿ�
}

// ���̿��� �浹 ����
bool OBBBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) {

    // _boundingBox�� ����Ͽ� ���̿��� �浹�� �����ϰ� ����� ��ȯ
    return _boundingBox.Intersects(rayOrigin, rayDir, OUT distance);
}

// �ٸ� �浹 ��������� �浹 ����
bool OBBBoxCollider::Intersects(const shared_ptr<BaseCollider>& other) {

    // �ٸ� �浹 �������� Ÿ���� Ȯ��
    ColliderType type = other->GetColliderType();

    // Ÿ�Կ� ���� ������ �浹 ���� �޼ҵ� ȣ��
    switch (type) {
    case ColliderType::Sphere: // ������ �浹
        return _boundingBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
    case ColliderType::AABB: // AABB���� �浹
        return _boundingBox.Intersects(dynamic_pointer_cast<AABBBoxCollider>(other)->GetBoundingBox());
    case ColliderType::OBB: // �ٸ� OBB���� �浹
        return _boundingBox.Intersects(dynamic_pointer_cast<OBBBoxCollider>(other)->GetBoundingBox());
    }

    return false; // �������� �ʴ� �浹 ������ Ÿ���� ��� false ��ȯ
}