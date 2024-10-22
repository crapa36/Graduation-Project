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

    float width = static_cast<float>(GEngine->GetWindow().clientWidth);
    float height = static_cast<float>(GEngine->GetWindow().clientHeight);

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
        if (!gameObject->IsEnable())
            continue;

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

    auto colliderA = objA->GetCollider();
    auto colliderB = objB->GetCollider();
    Vec3 collisionNormal = colliderA->GetCollisionNormal(colliderB);
    float collisionDepth = colliderA->GetCollisionDepth(colliderB);

    // 충돌 처리 로직
    ApplyCollisionResponse(objA, objB, collisionNormal, collisionDepth);

    // 충돌이 발생하지 않도록 위치 조정
    if (collisionDepth > 0.0f) {
        auto transformA = objA->GetTransform();
        auto transformB = objB->GetTransform();
        auto positionA = transformA->GetLocalPosition();
        auto positionB = transformB->GetLocalPosition();
        float MassA = 0.0f;
        float MassB = 0.0f;
        if (auto RigidbodyA = objA->GetRigidbody()) {
            MassA = RigidbodyA->GetMass();
        }

        if (auto RigidbodyB = objB->GetRigidbody()) {
            MassB = RigidbodyB->GetMass();
        }

        //Mass의 비율에 따라 위치 조정
        Vec3 adjustmentA = collisionNormal * (collisionDepth * (MassB / (MassA + MassB)));
        Vec3 adjustmentB = collisionNormal * (collisionDepth * (MassA / (MassA + MassB)));
        /*Vec3 adjustmentA = collisionNormal * (collisionDepth / 2.0f);
        Vec3 adjustmentB = collisionNormal * (collisionDepth / 2.0f);*/

        // 위치 조정
        if (objA->GetRigidbody() && objB->GetRigidbody()) {
            positionA -= adjustmentA;
            positionB += adjustmentB;

            transformA->SetLocalPosition(positionA);
            transformB->SetLocalPosition(positionB);
        }
        else if (objA->GetRigidbody()) {
            positionA -= adjustmentB;
            transformA->SetLocalPosition(positionA);
        }
        else if (objB->GetRigidbody()) {
            positionB += adjustmentA;
            transformB->SetLocalPosition(positionB);
        }
    }

    // 충돌 시간 업데이트
    _collisionCooldowns[collisionPair] = _cooldownDuration;
}
void PhysicsManager::UpdatePhysics() {
    auto& gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    std::vector<std::shared_ptr<GameObject>> terrains;

    // Terrain 객체를 미리 필터링하여 저장
    for (const auto& gameObject : gameObjects) {
        if (gameObject->GetTerrain()) {
            terrains.push_back(gameObject);
        }
    }

    size_t gameObjectCount = gameObjects.size();

    for (size_t i = 0; i < gameObjectCount; ++i) {
        const auto& gameObject = gameObjects[i];

        if (!gameObject->IsEnable())
            continue;
        auto collider = gameObject->GetCollider();
        auto rigidbody = gameObject->GetRigidbody();

        if (!collider)
            continue;

        for (size_t j = i + 1; j < gameObjectCount; ++j) {
            const auto& otherGameObject = gameObjects[j];

            if (IsParentChildRelationship(gameObject, otherGameObject)) {
                continue;
            }

            auto otherCollider = otherGameObject->GetCollider();

            if (!otherCollider)
                continue;

            if (collider->Intersects(otherCollider)) {
                HandleCollision(gameObject, otherGameObject);
            }
        }

        if (rigidbody) {
            const auto& transform = gameObject->GetTransform();
            Vec3 position = transform->GetLocalPosition();
            Vec4 rayOrigin(position.x, position.y, position.z, 1.0f);
            Vec3 colliderCenter = gameObject->GetCollider()->GetCenter();
            rayOrigin.x += colliderCenter.x;
            rayOrigin.y += colliderCenter.y;
            rayOrigin.z += colliderCenter.z;
            rayOrigin.y -= gameObject->GetCollider()->GetHeight();

            Vec4 rayDir(0.0f, -1.0f, 0.0f, 0.0f);

            for (const auto& terrain : terrains) {
                const auto& terrainTransform = terrain->GetTransform();
                const auto& terrainPosition = terrainTransform->GetLocalPosition();
                const auto& terrainScale = terrainTransform->GetLocalScale();

                float height = GetInterpolatedHeightAtPosition(terrain->GetTerrain(), rayOrigin.x - terrainPosition.x, rayOrigin.z - terrainPosition.z);
                float heightValue = terrainScale.y * height + terrainPosition.y;
                float distance = 0.0f;

                auto terrainCollider = terrain->GetCollider();
                if (terrainCollider->Intersects(rayOrigin, rayDir, OUT distance) && (heightValue - terrainPosition.y > distance)) {

                    // 주변 높이를 통해 법선 벡터 계산
                    float heightL = GetInterpolatedHeightAtPosition(terrain->GetTerrain(), rayOrigin.x - terrainPosition.x - 1.0f, rayOrigin.z - terrainPosition.z);
                    float heightR = GetInterpolatedHeightAtPosition(terrain->GetTerrain(), rayOrigin.x - terrainPosition.x + 1.0f, rayOrigin.z - terrainPosition.z);
                    float heightD = GetInterpolatedHeightAtPosition(terrain->GetTerrain(), rayOrigin.x - terrainPosition.x, rayOrigin.z - terrainPosition.z - 1.0f);
                    float heightU = GetInterpolatedHeightAtPosition(terrain->GetTerrain(), rayOrigin.x - terrainPosition.x, rayOrigin.z - terrainPosition.z + 1.0f);

                    Vec3 normal;
                    normal.x = heightL - heightR;
                    normal.y = 2.0f;
                    normal.z = heightD - heightU;
                    normal.Normalize();

                    Vec3 velocity = rigidbody->GetVelocity();
                    float dot = velocity.Dot(normal);
                    Vec3 reflection = velocity - 2 * dot * normal;

                    // 반발력 적용
                    rigidbody->SetVelocity(reflection * 0.5f); // 반발 계수 적용

                    // 위치 조정
                    position.y = heightValue + gameObject->GetCollider()->GetHeight() - colliderCenter.y;
                    transform->SetLocalPosition(position);

                    ApplyCollisionResponse(gameObject, terrain, normal, 0.0f);
                    break;
                }
            }
        }
    }
}

bool PhysicsManager::Raycast(const Vec4& origin, const Vec4& direction, float maxDistance, RaycastHit* hitInfo) {
    auto& gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    bool hitDetected = false;
    float closestDistance = maxDistance;

    for (const auto& gameObject : gameObjects) {
        auto collider = gameObject->GetCollider();
        if (!collider)
            continue;

        float distance = 0.0f;
        if (collider->Intersects(origin, direction, OUT distance) && distance < closestDistance) {
            closestDistance = distance;
            hitDetected = true;

            if (hitInfo) {
                hitInfo->distance = distance;
                hitInfo->point = origin + direction * distance;
                hitInfo->normal = collider->GetCollisionNormal(origin, direction);
                hitInfo->gameObject = gameObject; // 충돌한 객체를 저장
            }
        }
    }

    return hitDetected;
}
void PhysicsManager::ApplyCollisionResponse(const shared_ptr<GameObject>& A, const shared_ptr<GameObject>& B, const Vec3& collisionNormal, float collisionDepth) {
    if (!A->GetCollider() || !B->GetCollider()) {
        return; // 충돌체가 없으면 충돌 처리를 하지 않음
    }

    auto rigidbodyA = A->GetRigidbody();
    auto rigidbodyB = B->GetRigidbody();

    // A와 B 중 한쪽에만 리지드바디가 있을 때의 처리
    bool hasRigidbodyA = (rigidbodyA != nullptr);
    bool hasRigidbodyB = (rigidbodyB != nullptr);

    if (!hasRigidbodyA && !hasRigidbodyB) {
        return;  // 둘 다 리지드바디가 없으면 처리할 필요 없음
    }

    // 각 객체의 속도와 질량, 탄성 계수 가져오기
    Vec3 velocityA = hasRigidbodyA ? rigidbodyA->GetVelocity() : Vec3(0, 0, 0);
    Vec3 velocityB = hasRigidbodyB ? rigidbodyB->GetVelocity() : Vec3(0, 0, 0);

    float massA = hasRigidbodyA ? rigidbodyA->GetMass() : 0.0f;
    float massB = hasRigidbodyB ? rigidbodyB->GetMass() : 0.0f;

    float elasticityA = hasRigidbodyA ? rigidbodyA->GetElasticity() : 0.0f;
    float elasticityB = hasRigidbodyB ? rigidbodyB->GetElasticity() : 0.0f;
    float combinedElasticity = (elasticityA + elasticityB) * 0.5f;

    Vec3 relativeVelocity = velocityB - velocityA;
    float normalVelocity = relativeVelocity.Dot(collisionNormal);

    // 충돌을 통한 속도 변화 계산 (양쪽이 다 리지드바디일 때만 반발력 적용)
    if (normalVelocity < 0) {
        float restitution = combinedElasticity;

        // 질량 기반 반발 계산
        float impulseMagnitude = -(1.0f + restitution) * normalVelocity;
        impulseMagnitude /= (massA > 0 ? (1 / massA) : 0.0f) + (massB > 0 ? (1 / massB) : 0.0f);

        Vec3 impulse = impulseMagnitude * collisionNormal;

        if (hasRigidbodyA) {
            velocityA -= impulse / massA;
            rigidbodyA->SetVelocity(velocityA);
        }

        if (hasRigidbodyB) {
            velocityB += impulse / massB;
            rigidbodyB->SetVelocity(velocityB);
        }
    }

    // 충돌 깊이에 따른 위치 보정 (Position Correction)
    float totalMass = massA + massB;
    Vec3 positionCorrection = collisionNormal * (collisionDepth / totalMass);

    if (hasRigidbodyA) {
        auto transformA = A->GetTransform();
        transformA->SetLocalPosition(transformA->GetLocalPosition() - positionCorrection * massB);
    }

    if (hasRigidbodyB) {
        auto transformB = B->GetTransform();
        transformB->SetLocalPosition(transformB->GetLocalPosition() + positionCorrection * massA);
    }

    // A 또는 B가 Terrain과 충돌한 경우 Grounded 설정
    if (hasRigidbodyA && B->GetTerrain()) {
        rigidbodyA->SetGrounded(true);
    }

    if (hasRigidbodyB && A->GetTerrain()) {
        rigidbodyB->SetGrounded(true);
    }
}

bool PhysicsManager::IsParentChildRelationship(const std::shared_ptr<GameObject>& gameObject, const std::shared_ptr<GameObject>& otherGameObject) {
    auto parentA = gameObject->GetTransform()->GetParent().lock();
    auto parentB = otherGameObject->GetTransform()->GetParent().lock();

    return (parentA && (parentA == otherGameObject->GetTransform())) ||
        (parentB && (parentB == gameObject->GetTransform())) ||
        (parentA && parentB && (parentA == parentB));
}

float PhysicsManager::GetInterpolatedHeightAtPosition(const std::shared_ptr<Terrain>& terrain, float x, float z) {

    // 지형의 높이 맵에서 x, z 좌표에 대한 높이를 보간하여 계산
    // 예시로 bilinear interpolation을 사용
    int x0 = static_cast<int>(std::floor(x));
    int x1 = x0 + 1;
    int z0 = static_cast<int>(std::floor(z));
    int z1 = z0 + 1;

    float h00 = terrain->GetHeightAtPosition(x0, z0);
    float h10 = terrain->GetHeightAtPosition(x1, z0);
    float h01 = terrain->GetHeightAtPosition(x0, z1);
    float h11 = terrain->GetHeightAtPosition(x1, z1);

    float tx = x - x0;
    float tz = z - z0;

    float h0 = h00 * (1 - tx) + h10 * tx;
    float h1 = h01 * (1 - tx) + h11 * tx;

    return h0 * (1 - tz) + h1 * tz;
}