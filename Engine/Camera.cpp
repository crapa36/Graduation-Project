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

// ī�޶��� �� ��İ� ���� ����� ���� ����� �����մϴ�.
Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

// ī�޶� ������Ʈ�� �������Դϴ�.
Camera::Camera() : Component(COMPONENT_TYPE::CAMERA) {
}

// ī�޶� ������Ʈ�� �Ҹ����Դϴ�.
Camera::~Camera() {
}

// ī�޶��� ���� ������Ʈ �Լ��Դϴ�. �� ��İ� ���� ����� ����մϴ�.
void Camera::FinalUpdate() {

    // �� ����� ����մϴ�. ī�޶��� ���� ����� ������� ����մϴ�.
    _matView = GetTransform()->GetLocalToWorldMatrix().Invert();

    // �������� �ʺ�� ���̸� �����ɴϴ�.
    float width = static_cast<float>(GEngine->GetWindow().width);
    float height = static_cast<float>(GEngine->GetWindow().height);

    // ���� Ÿ�Կ� ���� ���� ����� ����մϴ�.
    if (_projectionType == PROJECTION_TYPE::PERSPECTIVE)
        _matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
    else
        _matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

    // ���������� ���� ������Ʈ�մϴ�.
    _frustum.FinalUpdate();
}

// ���� ������Ʈ�� �����ϴ� �Լ��Դϴ�.
void Camera::SortGameObject() {

    // ���� Ȱ��ȭ�� ���� �����ɴϴ�.
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    // �� ������ ���͸� �ʱ�ȭ�մϴ�.
    _vecForward.clear();
    _vecDeferred.clear();
    _vecParticle.clear();

    // ��� ���� ������Ʈ�� ��ȸ�մϴ�.
    for (auto& gameObject : gameObjects) {

        // �޽� �������� ��ƼŬ �ý����� ���� ������Ʈ�� �ǳʶݴϴ�.
        if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
            continue;

        // ���̾� �ε����� ���� �ø� ���θ� �����մϴ�.
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        // �������� �ø��� üũ�մϴ�.
        if (gameObject->GetCheckFrustum()) {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false) {
                continue;
            }
        }

        // �޽� �������� �ִ� ���, ���̴� Ÿ�Կ� ���� �з��մϴ�.
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

            // ��ƼŬ �ý����� �ִ� ���, ��ƼŬ ���Ϳ� �߰��մϴ�.
            _vecParticle.push_back(gameObject);
        }
    }
}

// ���۵� �������� �����ϴ� �Լ��Դϴ�.
void Camera::Render_Deferred() {

    // ���� ��� ������ �� ��İ� ���� ����� �����մϴ�.
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // �ν��Ͻ� �Ŵ����� ���� ���۵� ������ ����� �������մϴ�.
    GET_SINGLETON(InstancingManager)->Render(_vecDeferred);
}

// ������ �������� �����ϴ� �Լ��Դϴ�.
void Camera::Render_Forward() {

    // ���� ��� ������ �� ��İ� ���� ����� �����մϴ�.
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    // �ν��Ͻ� �Ŵ����� ���� ������ ������ ����� �������մϴ�.
    GET_SINGLETON(InstancingManager)->Render(_vecForward);

    // ��ƼŬ �ý����� �ִ� ���� ������Ʈ�� �������մϴ�.
    for (auto& gameObject : _vecParticle) {
        gameObject->GetParticleSystem()->Render();
    }
}