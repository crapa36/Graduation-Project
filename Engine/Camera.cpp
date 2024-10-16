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
#include "BaseCollider.h"
Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA) {
    _width = static_cast<float>(GEngine->GetWindow().clientWidth);
    _height = static_cast<float>(GEngine->GetWindow().clientHeight);
}

Camera::~Camera() {
}

void Camera::FinalUpdate() {
    _matView = GetTransform()->GetLocalToWorldMatrix().Invert();

    if (_projectionType == PROJECTION_TYPE::PERSPECTIVE)
        _matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
    else
        _matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);
    S_MatView = _matView;
    S_MatProjection = _matProjection;
    _frustum.FinalUpdate();
}

void Camera::SortGameObject() {
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    _vecForward.clear();
    _vecDeferred.clear();
    _vecParticle.clear();
#ifdef _DEBUG
    if (GEngine->GetDebugMode())
        _vecDebug.clear();
#endif

    for (auto& gameObject : gameObjects) {
        if (!gameObject->IsEnable())
            continue;
        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
            continue;

        if (gameObject->GetCheckFrustum()) {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false) {
                continue;
            }
        }
        
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
        if (gameObject->GetParticleSystem()) {
            _vecParticle.push_back(gameObject);
        }
#ifdef _DEBUG
        if (GEngine->GetDebugMode()) {
            if (gameObject->GetCollider()) {
                _vecDebug.push_back(gameObject);
            }
        }
#endif
    }
}

void Camera::SortShadowObject() {
    shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    _vecShadow.clear();

    for (auto& gameObject : gameObjects) {
        if (!gameObject->IsEnable())
            continue;
        if (gameObject->GetMeshRenderer() == nullptr)
            continue;

        if (gameObject->IsStatic())
            continue;

        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

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
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    GET_SINGLETON(InstancingManager)->Render(_vecDeferred);
}

void Camera::Render_Forward() {
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    GET_SINGLETON(InstancingManager)->Render(_vecForward);

    for (auto& gameObject : _vecParticle) {
        gameObject->GetParticleSystem()->Render();
    }
#ifdef _DEBUG
    if (GEngine->GetDebugMode()) {
        for (auto& gameObject : _vecDebug) {
            gameObject->GetCollider()->Render();
        }
    }
#endif
}

void Camera::Render_Shadow() {
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    for (auto& gameObject : _vecShadow) {
        gameObject->GetMeshRenderer()->RenderShadow();
    }
}

void Camera::Render_Reflection()
{
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    GET_SINGLETON(InstancingManager)->Render(_vecReflection);
}
