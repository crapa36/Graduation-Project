#pragma once

struct RaycastHit {
    float distance;
    Vec4 point;
    Vec4 normal;
    shared_ptr<class GameObject> gameObject;
};

class PhysicsManager {
    DECLARE_SINGLETON(PhysicsManager);

public:

    shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);
    void Update();
    void LateUpdate();
    void FinalUpdate();

    bool Raycast(const Vec4& origin, const Vec4& direction, float maxDistance, RaycastHit* hitInfo, const shared_ptr<GameObject>& excludedObject = NULL);

private:
    void HandleCollision(shared_ptr<GameObject> objA, shared_ptr<GameObject> objB);
    void UpdatePhysics();

    void HandleTerrainCollision(const shared_ptr<GameObject>& gameObject, const vector<shared_ptr<GameObject>>& terrains);

    void ApplyCollisionResponse(const shared_ptr<GameObject>& A, const shared_ptr<GameObject>& B, const Vec3& collisionNormal, float collisionDepth);

    bool IsParentChildRelationship(const shared_ptr<GameObject>& gameObject, const shared_ptr<GameObject>& otherGameObject);
    float GetInterpolatedHeightAtPosition(const shared_ptr<class Terrain>& terrain, float x, float z);
};