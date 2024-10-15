#include "pch.h"
#include "TitleScript.h"
#include "Input.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"

TitleScript::TitleScript() {
}

TitleScript::~TitleScript() {
}

void TitleScript::LateUpdate() {

    // 게임 윈도우가 포커스 되었을 때만 입력 처리
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {


        if (INPUT->IsMouseButtonPressed(0)) {  // 0은 왼쪽 마우스 버튼
            GET_SINGLETON(SceneManager)->LoadScene(L"TestScene");
        }
    }
}