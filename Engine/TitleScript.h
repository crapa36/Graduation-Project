#pragma once
#include "MonoBehaviour.h"

class TitleScript : public MonoBehaviour {
public:
    TitleScript();
    virtual ~TitleScript();

    virtual void LateUpdate() override;

private:
};
