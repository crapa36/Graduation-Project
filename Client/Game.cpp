#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Mouse.h"

void Game::Init(const WindowInfo& info) {
    GEngine->Init(info);
    GMouse->SetWindow(info.hwnd);

    m_keys.Reset();
    m_mouseButtons.Reset();

    GET_SINGLETON(SceneManager)->LoadScene(L"TestScene");
}

void Game::Update() {
    GEngine->Update();
}