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

// 카메라의 뷰 행렬과 투영 행렬을 정적 멤버로 선언합니다.
Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

// 카메라 컴포넌트의 생성자입니다.
Camera::Camera() : Component(COMPONENT_TYPE::CAMERA) {
}

// 카메라 컴포넌트의 소멸자입니다.
Camera::~Camera() {
}

// 카메라의 최종 업데이트 함수입니다. 뷰 행렬과 투영 행렬을 계산합니다.
void Camera::FinalUpdate() {

    // 뷰 행렬을 계산합니다. 카메라의 월드 행렬의 역행렬을 사용합니다.
    _matView = GetTransform()->GetLocalToWorldMatrix().Invert();

    // 윈도우의 너비와 높이를 가져옵니다.
    float width = static_cast<float>(GEngine->GetWindow().width);
    float height = static_cast<float>(GEngine->GetWindow().height);

    // 투영 타입에 따라 투영 행렬을 계산합니다.
    if (_projectionType == PROJECTION_TYPE::PERSPECTIVE)
        _matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
    else
        _matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

    // 프러스텀을 최종 업데이트합니다.
    _frustum.FinalUpdate();
}

// 게임 오브젝트를 정렬하는 함수입니다.
void Camera::SortGameObject() {

    // 현재 활성화된 씬을 가져옵니다.
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    // 각 렌더링 벡터를 초기화합니다.
    _vecForward.clear();
    _vecDeferred.clear();
    _vecParticle.clear();

    // 모든 게임 오브젝트를 순회합니다.
    for (auto& gameObject : gameObjects) {

        // 메시 렌더러나 파티클 시스템이 없는 오브젝트는 건너뜁니다.
        if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
            continue;

        // 레이어 인덱스에 따라 컬링 여부를 결정합니다.
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        // 프러스텀 컬링을 체크합니다.
        if (gameObject->GetCheckFrustum()) {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false) {
                continue;
            }
        }

        // 메시 렌더러가 있는 경우, 셰이더 타입에 따라 분류합니다.
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

            // 파티클 시스템이 있는 경우, 파티클 벡터에 추가합니다.
            _vecParticle.push_back(gameObject);
        }
    }
}

// 디퍼드 렌더링을 수행하는 함수입니다.
void Camera::Render_Deferred() {

    // 정적 멤버 변수에 뷰 행렬과 투영 행렬을 설정합니다.
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // 인스턴싱 매니저를 통해 디퍼드 렌더링 대상을 렌더링합니다.
    GET_SINGLETON(InstancingManager)->Render(_vecDeferred);
}

// 포워드 렌더링을 수행하는 함수입니다.
void Camera::Render_Forward() {

    // 정적 멤버 변수에 뷰 행렬과 투영 행렬을 설정합니다.
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // 인스턴싱 매니저를 통해 포워드 렌더링 대상을 렌더링합니다.
    GET_SINGLETON(InstancingManager)->Render(_vecForward);

    // 파티클 시스템이 있는 게임 오브젝트를 렌더링합니다.
    for (auto& gameObject : _vecParticle) {
        gameObject->GetParticleSystem()->Render();
    }
}