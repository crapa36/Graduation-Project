#pragma once
#include "Component.h"
#include "Frustum.h"

// 카메라의 투영 방식을 정의하는 열거형
enum class PROJECTION_TYPE {
    PERSPECTIVE, // 원근 투영
    ORTHOGRAPHIC, // 직교 투영
};

class Camera : public Component {
public:
    Camera();
    virtual ~Camera();

    virtual void FinalUpdate() override;

    // 투영 방식 설정 및 조회
    void SetProjectionType(PROJECTION_TYPE projectionType);
    PROJECTION_TYPE GetProjectionType();

    // 게임 오브젝트 및 그림자 오브젝트 정렬
    void SortGameObject();
    void SortShadowObject();

    // 렌더링 방식에 따른 렌더 함수
    void Render_Deferred();
    void Render_Forward();
    void Render_Shadow();

    // 레이어별 컬링 마스크 설정
    void SetCullingMaskLayerOnOff(uint8 layer, bool on);
    void SetCullingMaskAll();
    void SetCullingMask(uint32 mask);
    bool IsCulled(uint8 layer);

    // 카메라 설정 관련 함수
    void SetNear(float value);
    void SetFar(float value);
    void SetFOV(float value);
    void SetScale(float value);
    void SetWidth(float value);
    void SetHeight(float value);

    // 뷰 및 투영 행렬 조회
    Matrix& GetViewMatrix();
    Matrix& GetProjectionMatrix();

private:
    PROJECTION_TYPE _projectionType = PROJECTION_TYPE::PERSPECTIVE;

    // 카메라 설정 값
    float _near = 1.f;
    float _far = 1000.f;
    float _fov = XM_PI / 4.f;
    float _scale = 1.f;
    float _width = 0.f;
    float _height = 0.f;

    // 뷰 및 투영 행렬
    Matrix _matView = {};
    Matrix _matProjection = {};

    Frustum _frustum; // 프러스텀 컬링을 위한 객체
    uint32 _cullingMask = 0; // 컬링 마스크

private:

    // 렌더링 대상 게임 오브젝트 벡터
    vector<shared_ptr<GameObject>>	_vecDeferred;
    vector<shared_ptr<GameObject>>	_vecForward;
    vector<shared_ptr<GameObject>>	_vecParticle;
    vector<shared_ptr<GameObject>>	_vecShadow;

public:

    // 정적 뷰 및 투영 행렬
    static Matrix S_MatView;
    static Matrix S_MatProjection;
};
