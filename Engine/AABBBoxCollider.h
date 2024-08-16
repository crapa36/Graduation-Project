#pragma once

#include "BaseCollider.h" // �⺻ �浹 ������ Ŭ������ ��ӹޱ� ���� ����

class AABBBoxCollider : public BaseCollider {
public:

    // �⺻ ������
    AABBBoxCollider();

    // �Ҹ���
    virtual ~AABBBoxCollider();

    // �� �����Ӹ��� ȣ��Ǿ� ��ü ���¸� ������Ʈ�ϴ� �޼ҵ�
    virtual void Update() override;

    // ���̿��� �浹 ����
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;

    // �ٸ� �浹 ��������� �浹 ����
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    // ��� ���� ��������
    BoundingBox& GetBoundingBox() { return _boundingBox; } // ���� �浹 �������� AABB ��ȯ

private:
    BoundingBox _boundingBox; // ���������� ����ϴ� AABB
};