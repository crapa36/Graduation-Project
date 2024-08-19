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

shared_ptr<GameObject> PhysicsManager::Pick(int32 screenX, int32 screenY) {
    shared_ptr<Camera> camera = GET_SINGLETON(SceneManager)->GetActiveScene()->GetMainCamera();

    float width = static_cast<float>(GEngine->GetWindow().width);
    float height = static_cast<float>(GEngine->GetWindow().height);

    Matrix projectionMatrix = camera->GetProjectionMatrix();

    // ViewSpace에서 Picking 진행
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

        // ViewSpace에서의 Ray 정의
        Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

        // WorldSpace에서의 Ray 정의
        rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
        rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
        rayDir.Normalize();

        // WorldSpace에서 연산
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

void PhysicsManager::Gravity()
{
    auto& gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();

    Vec3 gravity = { 0.0f, -9.8f, 0.0f };

    vector<shared_ptr<GameObject>> Terrains;

    for (auto& gameObject : gameObjects) {
        if (gameObject->GetTerrain()) {
            Terrains.push_back(gameObject);
        }
    }

    for (auto& gameObject : gameObjects) {
        if (gameObject->IsGravity()) {
            Vec3 acceleration = gameObject->GetTransform()->GetAcceleration();
            Vec3 velocity = gameObject->GetTransform()->GetVelocity();
            acceleration = gravity;

            for (auto& terrain : Terrains) {
                Vec3 position = gameObject->GetTransform()->GetLocalPosition();
                Vec3 terrainPosition = terrain->GetTransform()->GetLocalPosition();
                Vec3 terrainScale = terrain->GetTransform()->GetLocalScale();
                Vec4 rayOrigin = Vec4(position.x, position.y, position.z, 1.0f);
                Vec4 rayDir = Vec4(0.0f, -1.0f, 0.0f, 0.0f);
                float height = terrain->GetTerrain()->GetHeightAtPosition(position.x, position.z);
                float heightValue = terrainScale.y * height + terrainPosition.y;    
                float distance = 0.f;
                if (terrain->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance)) {
                    if (heightValue - terrainPosition.y > distance) {
                        acceleration.y = 0.f;
                        velocity.y = 0.f;
                        position.y = heightValue;
                        gameObject->GetTransform()->SetLocalPosition(position);
                        break;
                    }
                }
            }

        gameObject->GetTransform()->SetAcceleration(acceleration);
        gameObject->GetTransform()->SetVelocity(velocity);
        }
    }


}
