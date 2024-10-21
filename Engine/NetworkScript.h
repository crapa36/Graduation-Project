#pragma once
#include "MonoBehaviour.h"

class NetworkScript : public MonoBehaviour
{
public:
    NetworkScript();
    virtual ~NetworkScript();

    virtual void Awake() override;
    virtual void Update() override;
};

