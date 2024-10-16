#include "pch.h"
#include "MenuScript.h"
#include "Input.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"




MenuScript::MenuScript() {
    
}

MenuScript::~MenuScript() {
}

void MenuScript::LateUpdate() {
    if (GetForegroundWindow() != GEngine->GetWindow().hwnd) return;

    // 마우스 좌클릭 시 선택 또는 피킹 처리
    if (INPUT->IsMouseButtonPressed(0)) {
        const POINT& mousePos = INPUT->GetMousePos();
        auto pickedObject = GET_SINGLETON(PhysicsManager)->Pick(mousePos.x, mousePos.y);
        if (pickedObject && pickedObject->GetName() == L"MenuExitButton") {
            pickedObject->SetEnable(false);
            GetGameObject()->SetEnable(false);
        }
    }

    // Alt 키 입력으로 마우스 잠금 전환


    // 마우스 휠로 UV 수정해 스크롤
    int mouseWheel = INPUT->GetMouseWheel();
    if (mouseWheel != 0) {
        auto material = GetGameObject()->GetMeshRenderer()->GetMaterial();
        Vec4 uv = material->GetVec4(0); // 현재 UV 값을 가져옴

        // 마우스 휠 값에 따라 UV 좌표를 수정
        uv.y -= mouseWheel * 0.001f; 

        if (uv.y < 0.0f) {

            uv.y = 0.0f;
        }
        else if (uv.y > 0.5f) {
            uv.y = 0.5f;
           
        }
        material->SetVec4(0, uv); // 수정된 UV 값을 설정
    }
}