#include <Windows.h>

#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Timer.h"
#include "PhysicsManager.h"

TestCameraScript::TestCameraScript() {
}

TestCameraScript::~TestCameraScript() {
}

void TestCameraScript::LateUpdate() {

    //���콺 

    auto mouse = GMouse->GetState();

    if (mouse.positionMode == Mouse::MODE_RELATIVE)
    {
        float offset = 0.0005f;
        Vec3 delta = Vec3(float(mouse.x), float(mouse.y), 0.f)
            * offset;

        Vec3 rotation = GetTransform()->GetLocalRotation();

        rotation.x += delta.y;
        rotation.y += delta.x;

        GetTransform()->SetLocalRotation(rotation);
    }

    GMouse->SetMode(mouse.leftButton
        ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

    //Ű����

    auto kb = GKeyboard->GetState();

    Vec3 pos = GetTransform()->GetLocalPosition();

    if (kb.W)
        pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (kb.S)
        pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (kb.A)
        pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (kb.D)
        pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

    GetTransform()->SetLocalPosition(pos);
}