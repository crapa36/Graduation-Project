#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA) {

    // 카메라의 초기 너비와 높이를 엔진의 윈도우 크기로 설정
    _width = static_cast<float>(GEngine->GetWindow().width);
    _height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera() {
}

void Camera::FinalUpdate() {

    // 뷰 행렬을 게임 오브젝트의 변환 정보를 기반으로 계산
    _matView = GetTransform()->GetLocalToWorldMatrix().Invert();

    // 투영 방식에 따라 투영 행렬 계산
    if (_projectionType == PROJECTION_TYPE::PERSPECTIVE)
        _matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
    else
        _matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);

    // 프러스텀 업데이트
    _frustum.FinalUpdate();
}

void Camera::SortGameObject() {

    // 활성 씬의 게임 오브젝트를 가져와 렌더링 대상으로 분류
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    // 벡터 초기화
    _vecForward.clear();
    _vecDeferred.clear();
    _vecParticle.clear();

    for (auto& gameObject : gameObjects) {

        // 메시 렌더러나 파티클 시스템이 없는 경우 건너뜀
        if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
            continue;

        // 컬링 마스크에 의해 컬링된 경우 건너뜀
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        // 프러스텀 컬링 체크
        if (gameObject->GetCheckFrustum()) {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false) {
                continue;
            }
        }

        // 셰이더 타입에 따라 분류
        if (gameObject->GetMeshRenderer()) {
            SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
            switch (shaderType) {
            case SHADER_TYPE::DEFERRED:
                _vecDeferred.push_back(gameObject);
                break;
            case SHADER_TYPE::FORWARD:
                _vecForward.push_back(gameObject);
                break;
            }
        }
        else {
            _vecParticle.push_back(gameObject);
        }
    }
}

void Camera::SortShadowObject() {

    // 그림자를 그릴 게임 오브젝트 분류
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    _vecShadow.clear();

    for (auto& gameObject : gameObjects) {

        // 메시 렌더러가 없거나 정적인 경우 건너뜀
        if (gameObject->GetMeshRenderer() == nullptr || gameObject->IsStatic())
            continue;

        // 컬링 마스크에 의해 컬링된 경우 건너뜀
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        // 프러스텀 컬링 체크
        if (gameObject->GetCheckFrustum()) {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false) {
                continue;
            }
        }

        _vecShadow.push_back(gameObject);
    }
}

void Camera::Render_Deferred() {

    // 디퍼드 렌더링을 위한 뷰 및 투영 행렬 설정
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // 인스턴싱 매니저를 통해 디퍼드 렌더링 대상 렌더
    GET_SINGLETON(InstancingManager)->Render(_vecDeferred);
}

void Camera::Render_Forward() {

    // 포워드 렌더링을 위한 뷰 및 투영 행렬 설정
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // 인스턴싱 매니저를 통해 포워드 렌더링 대상 및 파티클 렌더
    GET_SINGLETON(InstancingManager)->Render(_vecForward);

    for (auto& gameObject : _vecParticle) {
        gameObject->GetParticleSystem()->Render();
    }
}

void Camera::Render_Shadow() {

    // 그림자 렌더링을 위한 뷰 및 투영 행렬 설정
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // 그림자를 그릴 게임 오브젝트 렌더
    for (auto& gameObject : _vecShadow) {
        gameObject->GetMeshRenderer()->RenderShadow();
    }
}