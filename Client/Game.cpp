#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"

void Game::Init(const WindowInfo& info) {
    GEngine->Init(info);

    GET_SINGLETON(SceneManager)->LoadScene(L"TestScene");
}

void Game::Update() {
    GEngine->Update();
}