#pragma once

struct TupleHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::tuple<std::shared_ptr<T1>, std::shared_ptr<T2>>& tuple) const {
        auto h1 = std::hash<std::shared_ptr<T1>>{}(std::get<0>(tuple));
        auto h2 = std::hash<std::shared_ptr<T2>>{}(std::get<1>(tuple));
        return h1 ^ (h2 << 1);
    }
};

struct TupleEqual {
    template <typename T1, typename T2>
    bool operator()(const std::tuple<std::shared_ptr<T1>, std::shared_ptr<T2>>& lhs,
                    const std::tuple<std::shared_ptr<T1>, std::shared_ptr<T2>>& rhs) const {
        return std::get<0>(lhs) == std::get<0>(rhs) && std::get<1>(lhs) == std::get<1>(rhs);
    }
};

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

    bool Raycast(const Vec4& origin, const Vec4& direction, float maxDistance, RaycastHit* hitInfo);

    void ApplyCollisionResponse(const shared_ptr<GameObject>& A, const shared_ptr<GameObject>& B, const Vec3& collisionNormal, float collisionDepth);

private:
    void HandleCollision(shared_ptr<GameObject> objA, shared_ptr<GameObject> objB);
    void UpdatePhysics();

    std::unordered_map<std::tuple<std::shared_ptr<GameObject>, std::shared_ptr<GameObject>>, float, TupleHash, TupleEqual> _collisionCooldowns;
    const float _cooldownDuration = 0.01f;
};