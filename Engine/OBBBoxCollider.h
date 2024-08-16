#pragma once
#include "BaseCollider.h" // �⺻ �浹 ������ Ŭ���� ����

// ��ü ���� ��� ���ڸ� ��Ÿ���� Ŭ����
class OBBBoxCollider : public BaseCollider {
public:

    // ������
    OBBBoxCollider();

    // �Ҹ���
    virtual ~OBBBoxCollider();

    // �� �����Ӹ��� �浹 ������ ���� ������Ʈ
    virtual void Update() override;

    // ���̿� �浹 ���� ���� �Ǵ�
    virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) override;

    // �ٸ� �浹 ��������� ���� ���� �Ǵ�
    virtual bool Intersects(const shared_ptr<BaseCollider>& other) override;

    // ��� ���� ��������
    BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }

private:
    BoundingOrientedBox _boundingBox; // ��� ����
};