#include "pch.h"
#include "PhysicsManager.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "Input.h"
#include "Engine.h"
#include "Transform.h"
#include "Terrain.h"
#include "Scene.h"
#include "BaseCollider.h"
#include "Rigidbody.h"

shared_ptr<GameObject> PhysicsManager::Pick(int32 screenX, int32 screenY) {
    shared_ptr<Camera> camera = GET_SINGLETON(SceneManager)->GetActiveScene()->GetMainCamera();

    float width = static_cast<float>(GEngine->GetWindow().width);
    float height = static_cast<float>(GEngine->GetWindow().height);

    Matrix projectionMatrix = camera->GetProjectionMatrix();

    // ViewSpace���� Picking ����
    float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
    float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix viewMatrixInv = viewMatrix.Invert();

    auto& gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();

    float minDistance = FLT_MAX;
    shared_ptr<GameObject> picked;

    for (auto& gameObject : gameObjects) {
        if (gameObject->GetCollider() == nullptr)
            continue;

        // ViewSpace������ Ray ����
        Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

        // WorldSpace������ Ray ����
        rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
        rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
        rayDir.Normalize();

        // WorldSpace���� ����
        float distance = 0.f;
        if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
            continue;

        if (distance < minDistance) {
            minDistance = distance;
            picked = gameObject;
        }
    }

    return picked;
}

void PhysicsManager::Update() {
    Collision();
}
void PhysicsManager::FinalUpdate() {
}

void PhysicsManager::Collision() {
    auto& gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    for (auto& gameObject : gameObjects) {
        if (gameObject->GetCollider()) {
            for (const shared_ptr<GameObject>& otherGameObject : gameObjects) {
                if (otherGameObject->GetCollider() && gameObject != otherGameObject) {
                    if (gameObject->GetCollider()->Intersects(otherGameObject->GetCollider())) {
                        if (gameObject->GetRigidbody()) {
                            gameObject->GetRigidbody()->OnCollisionEnter(otherGameObject->GetCollider());
                        }

                        if (otherGameObject->GetRigidbody()) {
                            otherGameObject->GetRigidbody()->OnCollisionEnter(gameObject->GetCollider());
                        }
                    }
                }
            }
        }
    }
}