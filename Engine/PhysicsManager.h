#pragma once
class PhysicsManager {
    DECLARE_SINGLETON(PhysicsManager);

public:

    shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);
};