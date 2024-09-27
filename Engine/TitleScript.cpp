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

    // ���� �����찡 ��Ŀ�� �Ǿ��� ���� �Է� ó��
    if (GetForegroundWindow() == GEngine->GetWindow().hwnd) {

        // ���콺 ��Ŭ�� �� ���� �Ǵ� ��ŷ ó��
        if (INPUT->IsMouseButtonPressed(0)) {  // 0�� ���� ���콺 ��ư
            GET_SINGLETON(SceneManager)->LoadScene(L"TestScene");
        }
    }
}