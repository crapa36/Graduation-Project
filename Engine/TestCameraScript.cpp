#include <Windows.h>

#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Timer.h"
#include "Engine.h"
#include "PhysicsManager.h"

TestCameraScript::TestCameraScript() {
}

TestCameraScript::~TestCameraScript() {
}

void TestCameraScript::LateUpdate() {
    //마우스, 키보드 상태 불러오기
    auto mouse = GMouse->GetState();
    m_mouseButtons.Update(mouse);
    auto kb = GKeyboard->GetState();
    m_keys.Update(kb);

    //마우스 
    GMouse->ResetScrollWheelValue();

    GMouse->SetMode(Mouse::MODE_RELATIVE);

    if (mouse.positionMode == Mouse::MODE_RELATIVE)
    {
        float offset = 0.005f;
        Vec3 rotation = GetTransform()->GetLocalRotation();

        rotation.x += mouse.y * offset;
        rotation.y += mouse.x * offset;

        // 회전 각도 제한 (옵션)
        rotation.x = std::clamp(rotation.x, -XM_PIDIV2, XM_PIDIV2);
        rotation.y = std::fmod(rotation.y, XM_2PI);

        if (mouse.x != 0 && mouse.y != 0)
            std::cout << mouse.x << " , " << mouse.y << endl;

        GetTransform()->SetLocalRotation(rotation);
    }
    

    //키보드

    Vec3 pos = GetTransform()->GetLocalPosition();

    if (kb.W)
        pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (kb.S)
        pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

    if (kb.A)
        pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

    if (kb.D)
        pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

    GetTransform()->SetLocalPosition(pos);
}