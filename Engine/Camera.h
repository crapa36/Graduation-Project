#pragma once
#include "Component.h"
#include "Frustum.h"

// ī�޶��� ���� ����� �����ϴ� ������
enum class PROJECTION_TYPE {
    PERSPECTIVE, // ���� ����
    ORTHOGRAPHIC, // ���� ����
};

class Camera : public Component {
public:
    Camera();
    virtual ~Camera();

    virtual void FinalUpdate() override;

    // ���� ��� ���� �� ��ȸ
    void SetProjectionType(PROJECTION_TYPE projectionType);
    PROJECTION_TYPE GetProjectionType();

    // ���� ������Ʈ �� �׸��� ������Ʈ ����
    void SortGameObject();
    void SortShadowObject();

    // ������ ��Ŀ� ���� ���� �Լ�
    void Render_Deferred();
    void Render_Forward();
    void Render_Shadow();

    // ���̾ �ø� ����ũ ����
    void SetCullingMaskLayerOnOff(uint8 layer, bool on);
    void SetCullingMaskAll();
    void SetCullingMask(uint32 mask);
    bool IsCulled(uint8 layer);

    // ī�޶� ���� ���� �Լ�
    void SetNear(float value);
    void SetFar(float value);
    void SetFOV(float value);
    void SetScale(float value);
    void SetWidth(float value);
    void SetHeight(float value);

    // �� �� ���� ��� ��ȸ
    Matrix& GetViewMatrix();
    Matrix& GetProjectionMatrix();

private:
    PROJECTION_TYPE _projectionType = PROJECTION_TYPE::PERSPECTIVE;

    // ī�޶� ���� ��
    float _near = 1.f;
    float _far = 1000.f;
    float _fov = XM_PI / 4.f;
    float _scale = 1.f;
    float _width = 0.f;
    float _height = 0.f;

    // �� �� ���� ���
    Matrix _matView = {};
    Matrix _matProjection = {};

    Frustum _frustum; // �������� �ø��� ���� ��ü
    uint32 _cullingMask = 0; // �ø� ����ũ

private:

    // ������ ��� ���� ������Ʈ ����
    vector<shared_ptr<GameObject>>	_vecDeferred;
    vector<shared_ptr<GameObject>>	_vecForward;
    vector<shared_ptr<GameObject>>	_vecParticle;
    vector<shared_ptr<GameObject>>	_vecShadow;

public:

    // ���� �� �� ���� ���
    static Matrix S_MatView;
    static Matrix S_MatProjection;
};
