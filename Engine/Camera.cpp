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

    // ī�޶��� �ʱ� �ʺ�� ���̸� ������ ������ ũ��� ����
    _width = static_cast<float>(GEngine->GetWindow().width);
    _height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera() {
}

void Camera::FinalUpdate() {

    // �� ����� ���� ������Ʈ�� ��ȯ ������ ������� ���
    _matView = GetTransform()->GetLocalToWorldMatrix().Invert();

    // ���� ��Ŀ� ���� ���� ��� ���
    if (_projectionType == PROJECTION_TYPE::PERSPECTIVE)
        _matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
    else
        _matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);

    // �������� ������Ʈ
    _frustum.FinalUpdate();
}

void Camera::SortGameObject() {

    // Ȱ�� ���� ���� ������Ʈ�� ������ ������ ������� �з�
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    // ���� �ʱ�ȭ
    _vecForward.clear();
    _vecDeferred.clear();
    _vecParticle.clear();

    for (auto& gameObject : gameObjects) {

        // �޽� �������� ��ƼŬ �ý����� ���� ��� �ǳʶ�
        if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
            continue;

        // �ø� ����ũ�� ���� �ø��� ��� �ǳʶ�
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        // �������� �ø� üũ
        if (gameObject->GetCheckFrustum()) {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false) {
                continue;
            }
        }

        // ���̴� Ÿ�Կ� ���� �з�
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

    // �׸��ڸ� �׸� ���� ������Ʈ �з�
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    _vecShadow.clear();

    for (auto& gameObject : gameObjects) {

        // �޽� �������� ���ų� ������ ��� �ǳʶ�
        if (gameObject->GetMeshRenderer() == nullptr || gameObject->IsStatic())
            continue;

        // �ø� ����ũ�� ���� �ø��� ��� �ǳʶ�
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        // �������� �ø� üũ
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

    // ���۵� �������� ���� �� �� ���� ��� ����
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // �ν��Ͻ� �Ŵ����� ���� ���۵� ������ ��� ����
    GET_SINGLETON(InstancingManager)->Render(_vecDeferred);
}

void Camera::Render_Forward() {

    // ������ �������� ���� �� �� ���� ��� ����
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // �ν��Ͻ� �Ŵ����� ���� ������ ������ ��� �� ��ƼŬ ����
    GET_SINGLETON(InstancingManager)->Render(_vecForward);

    for (auto& gameObject : _vecParticle) {
        gameObject->GetParticleSystem()->Render();
    }
}

void Camera::Render_Shadow() {

    // �׸��� �������� ���� �� �� ���� ��� ����
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // �׸��ڸ� �׸� ���� ������Ʈ ����
    for (auto& gameObject : _vecShadow) {
        gameObject->GetMeshRenderer()->RenderShadow();
    }
}