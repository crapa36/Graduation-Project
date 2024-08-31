//TODO : 충돌 오브젝트 리스트를 추가해 성능 최적화
//       추가적인 성능 최적화 알고리즘 도입 필요

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
#include "Timer.h"

shared_ptr<GameObject> PhysicsManager::Pick(int32 screenX, int32 screenY) {
    shared_ptr<Camera> camera = GET_SINGLETON(SceneManager)->GetActiveScene()->GetMainCamera();

    float width = static_cast<float>(GEngine->GetWindow().width);
    float height = static_cast<float>(GEngine->GetWindow().height);

    Matrix projectionMatrix = camera->GetProjectionMatrix();

    // ViewSpace에서 Picking 수행
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

        // ViewSpace에서 Ray 생성
        Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

        // WorldSpace에서 Ray 생성
        rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
        rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
        rayDir.Normalize();

        // WorldSpace에서 충돌 검사
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

    // 쿨타임 업데이트
    for (auto& [key, cooldown] : _collisionCooldowns) {
        if (cooldown > 0.0f) {
            cooldown -= DELTA_TIME;
        }
    }
    UpdatePhysics();
}
void PhysicsManager::LateUpdate() {
}
void PhysicsManager::FinalUpdate() {
}

void PhysicsManager::HandleCollision(std::shared_ptr<GameObject> objA, std::shared_ptr<GameObject> objB) {
    auto collisionPair = std::make_tuple(objA, objB);

    // 충돌 쿨타임 체크
    if (_collisionCooldowns.find(collisionPair) != _collisionCooldowns.end()) {
        float& cooldown = _collisionCooldowns[collisionPair];
        if (cooldown > 0.0f) {

            // 쿨타임이 지나지 않았으므로 충돌 무시
            return;
        }
    }

    // 충돌 처리 로직
    if (auto Rigidbody = objA->GetRigidbody()) {
        Rigidbody->OnCollisionEnter(objB);
    }

    if (auto otherRigidbody = objB->GetRigidbody()) {
        otherRigidbody->OnCollisionEnter(objA);
    }

    // 충돌이 발생하지 않도록 위치 조정
    auto colliderA = objA->GetCollider();
    auto colliderB = objB->GetCollider();
    Vec3 collisionNormal = colliderA->GetCollisionNormal(colliderB);
    float collisionDepth = colliderA->GetCollisionDepth(colliderB);

    if (collisionDepth > 0.0f) {
        auto transformA = objA->GetTransform();
        auto transformB = objB->GetTransform();
        auto positionA = transformA->GetLocalPosition();
        auto positionB = transformB->GetLocalPosition();

        Vec3 adjustment = Vec3(collisionNormal.x, collisionNormal.y, collisionNormal.z) * (collisionDepth / 2.0f);

        positionA -= adjustment;
        positionB += adjustment;

        transformA->SetLocalPosition(positionA);
        transformB->SetLocalPosition(positionB);
    }

    // 충돌 시간 업데이트
    _collisionCooldowns[collisionPair] = _cooldownDuration;
}

void PhysicsManager::UpdatePhysics() {
    auto& gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    std::vector<shared_ptr<GameObject>> terrains;

    // Terrain 객체를 미리 필터링하여 저장
    for (const auto& gameObject : gameObjects) {
        if (gameObject->GetTerrain()) {
            terrains.push_back(gameObject);
        }
    }

    size_t gameObjectCount = gameObjects.size();

    for (size_t i = 0; i < gameObjectCount; ++i) {
        const auto& gameObject = gameObjects[i];
        auto collider = gameObject->GetCollider();
        auto rigidbody = gameObject->GetRigidbody();

        // Rigidbody와 Collider가 모두 없는 경우, 처리를 생략
        if (!collider)
            continue;

        // 충돌 검사
        if (collider && !gameObject->GetTerrain()) {
            for (size_t j = i + 1; j < gameObjectCount; ++j) {
                const auto& otherGameObject = gameObjects[j];
                auto otherCollider = otherGameObject->GetCollider();

                if (!otherCollider || otherGameObject->GetTerrain())
                    continue;

                if (collider->Intersects(otherCollider)) {
                    HandleCollision(gameObject, otherGameObject);
                }
            }
        }

        // 중력 적용 및 Terrain 충돌 검사
        if (rigidbody) {
            const auto& transform = gameObject->GetTransform();
            const auto& position = transform->GetLocalPosition();
            Vec4 rayOrigin(position.x, position.y, position.z, 1.0f);

            rayOrigin.y -= gameObject->GetCollider()->GetHeight();

            Vec4 rayDir(0.0f, -1.0f, 0.0f, 0.0f);

            // WorldSpace에서 충돌 검사
            for (const auto& terrain : terrains) {
                const auto& terrainTransform = terrain->GetTransform();
                const auto& terrainPosition = terrainTransform->GetLocalPosition();
                const auto& terrainScale = terrainTransform->GetLocalScale();

                float height = terrain->GetTerrain()->GetHeightAtPosition(position.x - terrainPosition.x, position.z - terrainPosition.z);
                float heightValue = terrainScale.y * height + terrainPosition.y;
                float distance = 0.f;

                if (terrain->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) && (heightValue - terrainPosition.y > distance)) {
                    if (gameObject->GetRigidbody()) {
                        gameObject->GetRigidbody()->OnCollisionEnter(terrain);
                    }
                    break;  // 한 Terrain과 충돌 시 나머지 Terrain 검사는 필요 없음
                }
            }
        }
    }
}